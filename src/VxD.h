#pragma once

#include <stdint.h>
#include <float.h>
#include <cmath>
#include <algorithm>
#include <smmintrin.h>

#define is16Aligned(mem) ( reinterpret_cast<uint64_t>(mem) & 15 == 0 ) 

#define _mm_castpd_si128(m128) ( *reinterpret_cast<__m128i*>(&m128) )

struct alignas(16) ViewMatrix { float mtx[4][4]; };

struct m128Matrix { __m128 r[4]; };

const extern m128Matrix umtx;
extern m128Matrix vmtx;

class V4D2;

class V4D
{
public:
	__m128 V_ = {};
	float* pV_ = reinterpret_cast<float*>(&V_);

	V4D() {}

	V4D(__m128 V)
	{
		this->V_ = V;
	}

	V4D(V4D2 V);

	V4D(const float V[4])
	{
		this->V_ = _mm_loadu_ps(V);
	}

	V4D(const float c1, const float c2, const float c3, const float c4)
	{
		this->V_ = _mm_set_ps(c4, c3, c2, c1);
	}

	V4D(const float c1, const float c2, const float c3)
	{
		this->V_ = _mm_set_ps(0, c3, c2, c1);
	}

	~V4D() {}

	V4D& operator = (const V4D& V)
	{
		this->V_ = V.V_;
		return *this;
	}

	float& operator [] (const int index) const
	{
		return this->pV_[index];
	}

	V4D operator + (const V4D& V) const
	{
		return _mm_add_ps(this->V_, V.V_);
	}

	void operator += (const V4D& V)
	{
		this->V_ = _mm_add_ps(this->V_, V.V_);
	}

	V4D operator - (V4D& V) const
	{
		return _mm_sub_ps(this->V_, V.V_);
	}

	void operator -= (V4D& V)
	{
		this->V_ = _mm_sub_ps(this->V_, V.V_);
	}

	inline float length() const
	{
		__m128 _V = _mm_mul_ps(this->V_, this->V_);
		_V = _mm_hadd_ps(_V, _V);
		_V = _mm_hadd_ps(_V, _V);
		return _mm_cvtss_f32(_mm_sqrt_ss(_V));
	}

	inline V4D scale(const float& S)
	{
		const __m128 _V = _mm_set1_ps(S);
		this->V_ = _mm_mul_ps(this->V_, _V);
		return *this;
	}

	inline V4D normalize()
	{
		__m128 _V = _mm_set1_ps(this->length());

		if (_mm_testz_si128(_mm_castps_si128(_V), _mm_set1_epi64x(0x7FFFFFFF7FFFFFFF)) == 0)
		{
			_V = _mm_rcp_ps(_V);
			this->V_ = _mm_mul_ps(this->V_, _V);
		};

		return *this;
	}

	inline float operator * (const V4D& V) const
	{
		return this->pV_[0] * V[0] + this->pV_[1] * V[1] + this->pV_[2] * V[2] + this->pV_[3] * V[3];
	}

	inline V4D cross(const V4D& V) const
	{
		const __m128 _V1 = this->V_;
		const __m128 _V2 = V.V_;
		__m128 V1 = _mm_shuffle_ps(_V1, _V1, _MM_SHUFFLE(3, 0, 2, 1));
		__m128 V2 = _mm_shuffle_ps(_V2, _V2, _MM_SHUFFLE(3, 1, 0, 2));
		__m128 V3 = _mm_mul_ps(V1, V2);
		V1 = _mm_shuffle_ps(_V2, _V2, _MM_SHUFFLE(3, 0, 2, 1));
		V2 = _mm_shuffle_ps(_V1, _V1, _MM_SHUFFLE(3, 1, 0, 2));
		static V4D V4 = _mm_sub_ps(V3, _mm_mul_ps(V1, V2));
		V4[3] = 0;
		return V4;
	}

	inline V4D component2V(const V4D& V) const
	{
		const V4D _V = _mm_mul_ps(V.V_, _mm_set1_ps(V * *this));
		return _V;
	}

	inline float sign2V(const V4D& V) const
	{
		__m128 _V = _mm_mul_ps(this->V_, V.V_);
		int32_t result = _mm_test_all_zeros(_mm_castps_si128(_V), _mm_set1_epi64x(0x8000000080000000));
		return static_cast<float>(2 * result - 1);
	}

	inline V4D transform(const V4D& V)
	{
		const float V0 = V[0];
		const float V1 = V[1];
		const float V2 = V[2];
		const float V3 = V[3];
		const float V_0 = this->pV_[0];
		const float V_1 = this->pV_[1];
		const float V_2 = this->pV_[2];
		this->pV_[0] = 2 * V0 * V0 * V_0 + 2 * V3 * V3 * V_0 + 2 * V1 * V0 * V_1 - 2 * V2 * V3 * V_1 + 2 * V0 * V2 * V_2 + 2 * V1 * V3 * V_2 - V_0;
		this->pV_[1] = 2 * V0 * V1 * V_0 + 2 * V2 * V3 * V_0 + 2 * V1 * V1 * V_1 + 2 * V3 * V3 * V_1 + 2 * V1 * V2 * V_2 - 2 * V0 * V3 * V_2 - V_1;
		this->pV_[2] = 2 * V0 * V2 * V_0 - 2 * V1 * V3 * V_0 + 2 * V1 * V2 * V_1 + 2 * V0 * V3 * V_1 + 2 * V2 * V2 * V_2 + 2 * V3 * V3 * V_2 - V_2;
		this->pV_[3] = 0;
		return *this;
	}

	inline void write(float* mem) const
	{
		_mm_store_ps(mem, this->V_);
	}
};

class V4D2
{
public:
	struct
	{
		__m128d H = {};
		__m128d L = {};
	} V_;
	double* pV_ = reinterpret_cast<double*>(&V_);

	V4D2() {}

	V4D2(__m128 V)
	{
		this->V_.H = _mm_cvtps_pd(V);
		this->V_.L = _mm_cvtps_pd(_mm_shuffle_ps(V, V, _MM_SHUFFLE(0, 1, 3, 2)));
	}

	V4D2(__m128d V1, __m128d V2)
	{
		this->V_.H = V1;
		this->V_.L = V2;
	}

	V4D2(V4D V)
	{
		__m128 _V = V.V_;
		this->V_.H = _mm_cvtps_pd(_V);
		this->V_.L = _mm_cvtps_pd(_mm_shuffle_ps(_V, _V, _MM_SHUFFLE(0, 1, 3, 2)));
	}

	V4D2(const float V[4])
	{
		__m128 _V = _mm_loadu_ps(V);
		this->V_.H = _mm_cvtps_pd(_V);
		this->V_.L = _mm_cvtps_pd(_mm_shuffle_ps(_V, _V, _MM_SHUFFLE(0, 1, 3, 2)));
	}

	V4D2(const double V[4])
	{
		this->V_.H = _mm_loadu_pd(V);
		this->V_.L = _mm_loadu_pd(V + 2);
	}

	V4D2(const double c1, const double c2, const double c3, const double c4)
	{
		this->V_.H = _mm_set_pd(c2, c1);
		this->V_.L = _mm_set_pd(c4, c3);
	}

	V4D2(const double c1, const double c2, const double c3)
	{
		this->V_.H = _mm_set_pd(c2, c1);
		this->V_.L = _mm_set_pd(0, c3);
	}

	V4D2(const double c1)
	{
		this->V_.H = _mm_set1_pd(c1);
		this->V_.L = this->V_.H;
	}

	V4D2(const V4D2 Axis, const double Angle)
	{
		const double SinA = sin(Angle / 2);
		const double CosA = cos(Angle / 2);
		this->V_.H = _mm_set_pd(Axis[1] * SinA, Axis[0] * SinA);
		this->V_.L = _mm_set_pd(CosA, Axis[2] * SinA);
	}

	V4D2(const V4D2 V1, const V4D2 V2)
	{
		const double LV1 = V1.length2();
		const double LV2 = V2.length2();
		*this = V1.cross(V2);
		this->pV_[3] = V1 * V2 + sqrt(LV1 * LV2);
	}

	~V4D2() {}

	V4D2& operator = (const V4D2& V)
	{
		this->V_ = V.V_;
		return *this;
	}

	V4D2& operator = (const double V[4])
	{
		this->V_.H = _mm_loadu_pd(V);
		this->V_.L = _mm_loadu_pd(V + 2);
		return *this;
	}

	constexpr double& operator [] (const int index) const
	{
		return this->pV_[index];
	}

	V4D2 operator + (const V4D2& V) const
	{
		V4D2 _V;
		_V.V_.H = _mm_add_pd(this->V_.H, V.V_.H);
		_V.V_.L = _mm_add_pd(this->V_.L, V.V_.L);
		return _V;
	}

	void operator += (const V4D2& V)
	{
		this->V_.H = _mm_add_pd(this->V_.H, V.V_.H);
		this->V_.L = _mm_add_pd(this->V_.L, V.V_.L);
	}

	V4D2 operator - (const V4D2& V) const
	{
		V4D2 _V;
		_V.V_.H = _mm_sub_pd(this->V_.H, V.V_.H);
		_V.V_.L = _mm_sub_pd(this->V_.L, V.V_.L);
		return _V;
	}

	void operator -= (const V4D2& V)
	{
		this->V_.H = _mm_sub_pd(this->V_.H, V.V_.H);
		this->V_.L = _mm_sub_pd(this->V_.L, V.V_.L);
	}

	V4D2 operator * (const double S) const
	{
		__m128d _V = _mm_set1_pd(S);
		V4D2 __V;
		__V.V_.H = _mm_mul_pd(this->V_.H, _V);
		__V.V_.L = _mm_mul_pd(this->V_.L, _V);
		return __V;
	}

	void operator *= (const double S)
	{
		__m128d _V = _mm_set1_pd(S);
		this->V_.H = _mm_mul_pd(this->V_.H, _V);
		this->V_.L = _mm_mul_pd(this->V_.L, _V);
	}

	inline double length() const
	{
		__m128d _VH = _mm_mul_pd(this->V_.H, this->V_.H);
		__m128d _VL = _mm_mul_pd(this->V_.L, this->V_.L);
		_VH = _mm_hadd_pd(_VH, _VL);
		_VH = _mm_hadd_pd(_VH, _VH);
		return _mm_cvtsd_f64(_mm_sqrt_pd(_VH));
	}

	inline double length2() const
	{
		__m128d _VH = _mm_mul_pd(this->V_.H, this->V_.H);
		__m128d _VL = _mm_mul_pd(this->V_.L, this->V_.L);
		_VH = _mm_hadd_pd(_VH, _VL);
		_VH = _mm_hadd_pd(_VH, _VH);
		return _mm_cvtsd_f64(_VH);
	}

	inline V4D2 scale(const double S)
	{
		__m128d _V = _mm_set1_pd(S);
		this->V_.H = _mm_mul_pd(this->V_.H, _V);
		this->V_.L = _mm_mul_pd(this->V_.L, _V);
		return *this;
	}

	inline V4D2 normalize()
	{
		__m128d _V = _mm_set1_pd(this->length());

		if (_mm_testz_si128(_mm_castpd_si128(_V), _mm_set1_epi64x(0x7FFFFFFFFFFFFFFF)) == 0)
		{
			const __m128d _all1 = _mm_set1_pd(1.0);
			_V = _mm_div_pd(_all1, _V);
			this->V_.H = _mm_mul_pd(this->V_.H, _V);
			this->V_.L = _mm_mul_pd(this->V_.L, _V);
		};

		return *this;
	}

	inline double operator * (const V4D2& V) const
	{
		return this->pV_[0] * V[0] + this->pV_[1] * V[1] + this->pV_[2] * V[2] + this->pV_[3] * V[3];
	}

	inline double dot3(const V4D2& V) const
	{
		return this->pV_[0] * V[0] + this->pV_[1] * V[1] + this->pV_[2] * V[2];
	}

	inline V4D2 cross(const V4D2& V) const
	{
		__m128d _V1H = this->V_.H;
		__m128d _V1L = this->V_.L;
		__m128d _V2H = V.V_.H;
		__m128d _V2L = V.V_.L;
		__m128d V1H = _mm_shuffle_pd(_V1H, _V1L, 0b00000001);
		__m128d V1L = _mm_shuffle_pd(_V1H, _V1L, 0b00000010);
		__m128d V2H = _mm_shuffle_pd(_V2L, _V2H, 0b00000000);
		__m128d V2L = _mm_shuffle_pd(_V2H, _V2L, 0b00000011);
		__m128d V3H = _mm_mul_pd(V1H, V2H); //2*7, 3*5
		__m128d V3L = _mm_mul_pd(V1L, V2L); //1*6, 0*0
		V1H = _mm_shuffle_pd(_V1L, _V1H, 0b00000000);
		V1L = _mm_shuffle_pd(_V1H, _V1L, 0b00000011);
		V2H = _mm_shuffle_pd(_V2H, _V2L, 0b00000001);
		V2L = _mm_shuffle_pd(_V2H, _V2L, 0b00000010);
		V4D2 V4(_mm_sub_pd(V3H, _mm_mul_pd(V1H, V2H)), _mm_sub_pd(V3L, _mm_mul_pd(V1L, V2L)));
		V4[3] = 0;
		return V4;
	}

	inline V4D2 component2V(const V4D2& V) const
	{
		__m128d k = _mm_set1_pd(V * *this);
		V4D2 _V;
		_V.V_.H = _mm_mul_pd(V.V_.H, k);
		_V.V_.L = _mm_mul_pd(V.V_.L, k);
		return _V;
	}

	inline double sign2V(const V4D2& V) const
	{
		__m128i sb = _mm_set1_epi64x(0x8000000000000000);
		__m128d _VH = _mm_mul_pd(this->V_.H, V.V_.H);
		__m128d _VL = _mm_mul_pd(this->V_.H, V.V_.H);
		int64_t result = _mm_test_all_zeros(_mm_castpd_si128(_VH), sb);
		return static_cast<double>(2 * result - 1);
	}

	inline V4D2 qTransform(const V4D2& V)
	{
		const double V0 = V[0];
		const double V1 = V[1];
		const double V2 = V[2];
		const double V3 = V[3];

		const double V_0 = this->pV_[0];
		const double V_1 = this->pV_[1];
		const double V_2 = this->pV_[2];

		const double V3V3_ = V3 * V3 - 0.5;

		const double V0V1 = V0 * V1;
		const double V0V2 = V0 * V2;
		const double V0V3 = V0 * V3;

		const double V1V2 = V1 * V2;
		const double V1V3 = V1 * V3;

		const double V2V3 = V2 * V3;

		this->pV_[0] = 2 * (V_0 * (V0 * V0 + V3V3_) + V_1 * (V0V1 - V2V3) + V_2 * (V0V2 + V1V3));
		this->pV_[1] = 2 * (V_0 * (V0V1 + V2V3) + V_1 * (V1 * V1 + V3V3_) + V_2 * (V1V2 - V0V3));
		this->pV_[2] = 2 * (V_0 * (V0V2 - V1V3) + V_1 * (V1V2 + V0V3) + V_2 * (V2 * V2 + V3V3_));
		this->pV_[3] = 0;
		return *this;
	}

	inline V4D2 qMul(const V4D2& V) const
	{
		const double V0 = this->pV_[0];
		const double V1 = this->pV_[1];
		const double V2 = this->pV_[2];
		const double V3 = this->pV_[3];

		const double V_0 = V[0];
		const double V_1 = V[1];
		const double V_2 = V[2];
		const double V_3 = V[3];

		return V4D2(V3 * V_0 + V0 * V_3 + V1 * V_2 - V2 * V_1,
					V3 * V_1 - V0 * V_2 + V1 * V_3 + V2 * V_0,
					V3 * V_2 + V0 * V_1 - V1 * V_0 + V2 * V_3,
					V3 * V_3 - V0 * V_0 - V1 * V_1 - V2 * V_2);
	}

	inline V4D2 qDiv(const V4D2& V) const
	{
		const double V0 = this->pV_[0];
		const double V1 = this->pV_[1];
		const double V2 = this->pV_[2];
		const double V3 = this->pV_[3];

		const double V_0 = V[0];
		const double V_1 = V[1];
		const double V_2 = V[2];
		const double V_3 = V[3];

		return V4D2(V_3 * V0 - V_0 * V3 + V_1 * V2 - V_2 * V1,
					V_3 * V1 - V_0 * V2 - V_1 * V3 + V_2 * V0,
					V_3 * V2 + V_0 * V1 - V_1 * V0 - V_2 * V3,
					V_3 * V3 + V_0 * V0 + V_1 * V1 + V_2 * V2);
	}

	inline V4D2 qPow(const double pow)
	{
		const double logFV = this->pV_[0] * this->pV_[0] + this->pV_[1] * this->pV_[1] + this->pV_[2] * this->pV_[2];
		const double logRV = sqrt(logFV);
		const double logFR = this->pV_[3];

		const double logS = logRV > DBL_EPSILON ? atan2(logRV, logFR) / logRV : 0.0;
		V4D2 logQ = this->pV_;
		logQ.scale(logS);
		logQ[3] = log(logFV + logFR * logFR) / 2;

		logQ.scale(pow);

		const double expFV = logQ[0] * logQ[0] + logQ[1] * logQ[1] + logQ[2] * logQ[2];
		const double expRV = sqrt(expFV);
		const double expRR = exp(logQ[3]);

		const double expS = expRV > DBL_EPSILON ? expRR * sin(expRV) / expRV : 0.0;
		*this = logQ;
		this->scale(expS);
		this->pV_[3] = expRR * cos(expRV);

		return *this;
	}

	inline V4D2 Slerp(const V4D2& V, const double t) const // http://number-none.com/product/Understanding%20Slerp,%20Then%20Not%20Using%20It/
	{
		V4D2 V1 = *this;
		double dot = V1 * V;

		if (dot > 0.9995) 
		{
			V4D2 V2 = V1 + (V - V1) * t;
			V2.normalize();
			return V2;
		}

		dot = std::clamp(dot, -1.0, 1.0);
		double theta_0 = acos(dot); 
		double theta = theta_0 * t;   

		V4D2 V2 = V - V1 * dot;
		V2.normalize();              

		return V1 * cos(theta) + V2 * sin(theta);
	}

	inline V4D2 aaVtoV_(const V4D2& AxAn, const double S = 1.0) const // https://mathb.in/51333
	{
		const double cosa = cos(AxAn[3] * S);
		const V4D2 cross = this->cross(AxAn);
		return (*this) * cosa + cross * sin(AxAn[3] * S) + AxAn * (AxAn.dot3(*this)) * (1.0 - cosa);
	}

	inline V4D2 aaAaB(const V4D2& AxAn1, const double S = 1.0) const
	{
		const double cosa2 = cos(this->pV_[3] / 2 * S);
		const double cosb2 = cos(AxAn1[3] / 2 * S);
		const double sina2 = sin(this->pV_[3] / 2 * S);
		const double sinb2 = sin(AxAn1[3] / 2 * S);

		const V4D2 Ax1 = V4D2(this->pV_[0], this->pV_[1], this->pV_[2]);
		const V4D2 Ax2 = V4D2(AxAn1[0], AxAn1[1], AxAn1[2]);

		const double An = acos(cosa2 * cosb2 - (Ax1.dot3(Ax2)) * sina2 * sinb2);

		V4D2 Ax3 = Ax1 * (sinb2 * cosb2) + Ax2 * (sina2 * sinb2) + Ax2.cross(Ax1) * (sina2 * sinb2);
		Ax3.normalize();
		Ax3[3] = An;
		return Ax3;
	}

	inline void write(float* mem) const
	{
		__m128 _VH = _mm_cvtpd_ps(this->V_.H);
		__m128 _VL = _mm_cvtpd_ps(this->V_.L);
		_mm_store_ps(mem, _mm_shuffle_ps(_VL, _VH, _MM_SHUFFLE(1, 0, 1, 0)));
	}
};

namespace VxD
{
	inline float length(const V4D& V)
	{
		return V.length();
	}

	inline V4D scale(const V4D& V, const float S)
	{
		V4D V_ = V;
		V_.scale(S);
		return V_;
	}

	inline V4D normalize(const V4D& V)
	{
		V4D V_ = V;
		V_.normalize();
		return V_;
	}

	inline V4D cross(const V4D& V1, const V4D& V2)
	{
		return V1.cross(V2);
	}

	inline V4D component2V(const V4D& V1, const V4D& V2)
	{
		return V1.component2V(V2);
	}

	inline float sign2V(const V4D& V1, const V4D& V2)
	{
		return V1.sign2V(V2);
	}

	inline V4D transform(const V4D& V1, const V4D& V2)
	{
		V4D V_ = V1;
		V_.transform(V2);
		return V_;
	}

	inline void write(const V4D& V, float* mem)
	{
		_mm_store_ps(mem, V.V_);
	}

	inline void write_vmtx(const V4D& V1, const V4D& V2, const V4D& V3, ViewMatrix* vmtx)
	{
		_mm_store_ps(vmtx->mtx[0], V1.V_);
		_mm_store_ps(vmtx->mtx[1], V2.V_);
		_mm_store_ps(vmtx->mtx[2], V3.V_);
	}

	inline void write_vmtx(const V4D& V1, const V4D& V2, const V4D& V3, const V4D& V4, ViewMatrix* vmtx)
	{
		_mm_store_ps(vmtx->mtx[0], V1.V_);
		_mm_store_ps(vmtx->mtx[1], V2.V_);
		_mm_store_ps(vmtx->mtx[2], V3.V_);
		_mm_store_ps(vmtx->mtx[3], V4.V_);
	}

	inline void write_umtx(ViewMatrix* vmtx)
	{
		_mm_store_ps(vmtx->mtx[0], umtx.r[0]);
		_mm_store_ps(vmtx->mtx[1], umtx.r[1]);
		_mm_store_ps(vmtx->mtx[2], umtx.r[2]);
		_mm_store_ps(vmtx->mtx[3], umtx.r[3]);
	}
}

namespace VxD2
{
	inline double length(const V4D2& V)
	{
		return V.length();
	}

	inline V4D2 scale(const V4D2& V, const double S)
	{
		V4D2 V_ = V;
		V_.scale(S);
		return V_;
	}

	inline V4D2 normalize(const V4D2& V)
	{
		V4D2 V_ = V;
		V_.normalize();
		return V_;
	}

	inline V4D2 cross(const V4D2& V1, const V4D2& V2)
	{
		return V1.cross(V2);
	}

	inline V4D2 component2V(const V4D2& V1, const V4D2& V2)
	{
		return V1.component2V(V2);
	}

	inline double sign2V(const V4D2& V1, const V4D2& V2)
	{
		return V1.sign2V(V2);
	}

	inline V4D2 qTransform(const V4D2& V1, const V4D2& V2)
	{
		V4D2 V_ = V1;
		V_.qTransform(V2);
		return V_;
	}

	inline V4D2 qDiv(const V4D2& V1, const V4D2& V2)
	{
		V4D2 V_ = V1;
		V_.qDiv(V2);
		return V_;
	}

	inline V4D2 qMul(const V4D2& V1, const V4D2& V2)
	{
		V4D2 V_ = V1;
		V_.qMul(V2);
		return V_;
	}

	inline V4D2 qFromMtx(const ViewMatrix& mtx) // https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/ethan.htm
	{
		__m128 mask = _mm_set_ps(-0.0f, -0.0f, -0.0f, 0.0f);

		__m128 c1 = _mm_xor_ps(_mm_set1_ps(mtx.mtx[0][0]), mask);
		__m128 c2 = _mm_xor_ps(_mm_set1_ps(mtx.mtx[1][1]), _mm_shuffle_ps(mask, mask, _MM_SHUFFLE(1, 1, 0, 1)));
		__m128 c3 = _mm_xor_ps(_mm_set1_ps(mtx.mtx[2][2]), _mm_shuffle_ps(mask, mask, _MM_SHUFFLE(1, 0, 1, 1)));


		__m128 Traces = _mm_add_ps(_mm_add_ps(_mm_add_ps(_mm_set1_ps(1.0f), c1), c2), c3);

		int TraceEnum = 0;
		__m128 TracesSort = Traces;
		for (int i = 1; i < 3; i++)
		{
			__m128 temp = TracesSort;
			TracesSort = _mm_shuffle_ps(TracesSort, TracesSort, _MM_SHUFFLE(3, 0, 2, 1));
			TracesSort = _mm_max_ps(Traces, TracesSort);
			if (!_mm_ucomieq_ss(temp, TracesSort))
			{
				TraceEnum += i;
			}
		}
		Traces = _mm_rsqrt_ss(TracesSort);

		float k = _mm_cvtss_f32(Traces) / 2.0f;

		V4D2 Q;

		switch (TraceEnum)
		{
		case 0:
			Q = { 0.25f / (k * k), mtx.mtx[0][1] + mtx.mtx[1][0], mtx.mtx[2][0] + mtx.mtx[0][2], mtx.mtx[2][1] - mtx.mtx[1][2] };
			Q *= k;
			break;
		case 1:
			Q = { mtx.mtx[0][1] + mtx.mtx[1][0], 0.25f / (k * k), mtx.mtx[1][2] + mtx.mtx[2][1], mtx.mtx[0][2] - mtx.mtx[2][0] };
			Q *= k;
			break;
		case 2:
		case 3:
			Q = { mtx.mtx[0][2] + mtx.mtx[2][0], mtx.mtx[1][2] + mtx.mtx[2][1], 0.25f / (k * k), mtx.mtx[1][0] - mtx.mtx[0][1] };
			Q *= k;
			break;
		}

		return Q;
	}

	inline void write(const V4D2& V, float* mem)
	{
		__m128 _VH = _mm_cvtpd_ps(V.V_.H);
		__m128 _VL = _mm_cvtpd_ps(V.V_.L);
		_mm_store_ps(mem, _mm_shuffle_ps(_VH, _VL, _MM_SHUFFLE(1, 0, 1, 0)));
	}

	inline void write_vmtx(const V4D2& V1, const V4D2& V2, const V4D2& V3, ViewMatrix* vmtx)
	{
		__m128 _VH = _mm_cvtpd_ps(V1.V_.H);
		__m128 _VL = _mm_cvtpd_ps(V1.V_.L);
		_mm_store_ps(vmtx->mtx[0], _mm_shuffle_ps(_VH, _VL, _MM_SHUFFLE(1, 0, 1, 0)));
		_VH = _mm_cvtpd_ps(V2.V_.H);
		_VL = _mm_cvtpd_ps(V2.V_.L);
		_mm_store_ps(vmtx->mtx[1], _mm_shuffle_ps(_VH, _VL, _MM_SHUFFLE(1, 0, 1, 0)));
		_VH = _mm_cvtpd_ps(V3.V_.H);
		_VL = _mm_cvtpd_ps(V3.V_.L);
		_mm_store_ps(vmtx->mtx[2], _mm_shuffle_ps(_VH, _VL, _MM_SHUFFLE(1, 0, 1, 0)));
	}

	inline void write_vmtx(const V4D2& V1, const V4D2& V2, const V4D2& V3, const V4D2& V4, ViewMatrix* vmtx)
	{
		write_vmtx(V1, V2, V3, vmtx);
		__m128 _VH = _mm_cvtpd_ps(V4.V_.H);
		__m128 _VL = _mm_cvtpd_ps(V4.V_.L);
		_mm_store_ps(vmtx->mtx[3], _mm_shuffle_ps(_VH, _VL, _MM_SHUFFLE(1, 0, 1, 0)));
	}

	inline void write_vmtx_c(const V4D2& V1, const V4D2& V2, const V4D2& V3, ViewMatrix* vmtx)
	{
		__m128 _VH1 = _mm_cvtpd_ps(V1.V_.H);
		__m128 _VL1 = _mm_cvtpd_ps(V1.V_.L);
		__m128 _VH2 = _mm_cvtpd_ps(V2.V_.H);
		__m128 _VL2 = _mm_cvtpd_ps(V2.V_.L);
		__m128 _VH3 = _mm_cvtpd_ps(V3.V_.H);
		__m128 _VL3 = _mm_cvtpd_ps(V3.V_.L);
		_mm_store_ps(vmtx->mtx[0], _mm_insert_ps(_mm_shuffle_ps(_VH2, _VH3, _MM_SHUFFLE(1, 0, 0, 1)), _VH1, 0));
		_mm_store_ps(vmtx->mtx[1], _mm_insert_ps(_mm_shuffle_ps(_VH2, _VH3, _MM_SHUFFLE(0, 1, 1, 0)), _mm_shuffle_ps(_VH1, _VH1, _MM_SHUFFLE(0, 0, 0, 1)), 0));
		_mm_store_ps(vmtx->mtx[2], _mm_insert_ps(_mm_shuffle_ps(_VL2, _VL3, _MM_SHUFFLE(1, 0, 0, 1)), _VL1, 0));
	}

	inline double traceToAn(const double c1, const double c2, const double c3)
	{
		return acos((c1 + c2 + c3 - 1) / 2);
	}

	inline V4D2 mtxToAx(const V4D2& V1, const V4D2& V2, const V4D2& V3)
	{
		ViewMatrix alignas(16) mtx = {};
		write_vmtx(V1, V2, V3, &mtx);

		const double k1 = 1 - mtx.mtx[0][1] + mtx.mtx[1][1] + mtx.mtx[2][1];
		const double k2 = 1 - mtx.mtx[0][2] + mtx.mtx[1][2] + mtx.mtx[2][2];
		const double k1k2 = k1 + k2;
		const double k3 = mtx.mtx[0][0] + mtx.mtx[1][0] + mtx.mtx[2][0] - 1;
		const double k32 = k3 * k3;
		const double k4 = (k2 * mtx.mtx[1][0] + k3 * mtx.mtx[1][2]) / (k1 * mtx.mtx[1][0] - k3);
		const double k42 = k4 * k4;
		const double k5 = sqrt(k42 * k42 + 2 * k42 + 4 * k1k2 * k1k2 * k42 / k32 + 1);

		const double Y = sqrt((k32 * (std::max(k5 - k42 - 1, 0.0)) / (k1k2 * k1k2 * k42 * 2)));
		const double Z = Y * k4;
		const double X = (Y * k1 + Z * k2) / k3;

		return V4D2(X, Z, Y);
	}
}