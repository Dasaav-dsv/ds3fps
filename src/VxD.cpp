#include "VxD.h"

const extern m128Matrix umtx = { { { 1.0f, 0.0f, 0.0f, 0.0f },
								   { 0.0f, 1.0f, 0.0f, 0.0f },
								   { 0.0f, 0.0f, 1.0f, 0.0f },
								   { 0.0f, 0.0f, 0.0f, 1.0f } } };

V4D::V4D(V4D2 V)
{
		__m128 _VH = _mm_cvtpd_ps(V.V_.H);
		__m128 _VL = _mm_cvtpd_ps(V.V_.L);
		this->V_ = _mm_shuffle_ps(_VH, _VL, _MM_SHUFFLE(1, 0, 1, 0));
}
