// Author: Alex Hartford
// Program: Simp
// File: Interp
// Date: April 2023

#ifndef INTERP_H
#define INTERP_H

float Lerp(float a, float b, float t) {
    return a * (1.0 - t) + (b * t);
}

vec3 Lerp(vec3 a, vec3 b, float t) {
    return a * (float)(1.0 - t) + (b * (float)t);
}

float Identity(float t) {
    return t;
}

float Flip(float t) {
    return 1 - t;
}

float SmoothStart2(float t) {
    return t * t;
}

float SmoothStart3(float t) {
    return t * t * t;
}

float SmoothStart4(float t) {
    return t * t * t * t;
}

float SmoothStart5(float t) {
    return t * t * t * t * t;
}

float SmoothStop2(float t) {
    return Flip(SmoothStart2(Flip(t)));
}

float SmoothStop3(float t) {
    return Flip(SmoothStart3(Flip(t)));
}

float SmoothStop4(float t) {
    return Flip(SmoothStart4(Flip(t)));
}

float SmoothStop5(float t) {
    return Flip(SmoothStart5(Flip(t)));
}

float SmoothStartStop(float t) {
    return Lerp(SmoothStart2(t), SmoothStop2(t), t);
}

float SmoothStartStopCustom(float t, float (*in) (float), float (*out) (float)) {
    return Lerp(in(t), out(t), t);
}

float Spike(float t) {
    if (t <= .5f)
        return SmoothStart2(t/0.5);
 
    return SmoothStart2(Flip(t)/0.5);
}

float Parabola(float t) {
    return t * (1 - t);
}

float BounceClampBottom(float t) {
    return fabs(t);
}

float BounceClampTop(float t) {
    return 1.0f - fabs(1.0f - t);
}

float BounceClampBottomTop(float t) {
    return BounceClampTop(BounceClampTop(t));
}

#endif
