// Author: Alex Hartford
// Program: Simp
// File: Curve
// Date: April 2023

#ifndef CURVE_H
#define CURVE_H

struct Line
{
    vector<vec3> points;
    unsigned int VAO, VBO;

    Line(vector<vec3> points_in)
    : points(points_in)
    {
        assert(points.size() > 1);
        BindBuffers();
    }

    void Draw(const Shader &shader) const {
        glBindVertexArray(VAO);
		glDrawArrays(GL_LINE_STRIP, 0, points.size());
        glBindVertexArray(0);
    }

    void BindBuffers() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(vec3), &points[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    }
};

// TODO: Rewrite this travesty of software engineering.
//       Put it out of its misery.
void spline(vector<vec3> &result_path, vector<vec3> &original_path,  int lod, float curly)
{	
	if (original_path.size()<3) return;
	result_path.clear();
	int n1 = original_path.size() + 1;
	vec3 *P, *d, *A;
	vec4 *B;

	double *Bi = NULL;
	P = new vec3[n1];
	d = new vec3[n1];
	A = new vec3[n1];
	B = new vec4[lod];

	//punkte setzen
	for (int ii = 0; ii<original_path.size(); ii++)
		P[ii] = original_path[ii];

	d[0] = vec3(0, 0, 0);
	d[original_path.size() - 1] = vec3(0, 0, 0);

	Bi = new double[n1];
	double t = 0;
	double tt = 1. / (lod - 1.);
	for (int i = 0; i< lod; i++)
	{
		double t1 = 1 - t, t12 = t1*t1, t2 = t*t;
		B[i] = vec4(t1*t12, 3 * t*t12, 3 * t2*t1, t*t2);
		t += tt;
	}

	//findpoints
	Bi[1] = -0.25*(4. / curly);

	A[1].x = (P[2].x - P[0].x - d[0].x) / 4;
	A[1].y = (P[2].y - P[0].y - d[0].y) / 4;
	A[1].z = (P[2].z - P[0].z - d[0].z) / 4;
	for (int i = 2; i < original_path.size() - 1; i++)
	{
		Bi[i] = -1 / (4 + Bi[i - 1]);
		A[i].x = -(P[i + 1].x - P[i - 1].x - A[i - 1].x)*Bi[i];
		A[i].y = -(P[i + 1].y - P[i - 1].y - A[i - 1].y)*Bi[i];
		A[i].z = -(P[i + 1].z - P[i - 1].z - A[i - 1].z)*Bi[i];
	}
	for (int i = original_path.size() - 2; i > 0; i--)
	{
		d[i].x = A[i].x + d[i + 1].x*Bi[i];
		d[i].y = A[i].y + d[i + 1].y*Bi[i];
		d[i].z = A[i].z + d[i + 1].z*Bi[i];
	}
	//points

	float X, Y, Z;
	float Xo = (float)P[0].x;
	float Yo = (float)P[0].y;
	//float Zo = (float)P[0].z;



	result_path.push_back(vec3(Xo, Yo, 0));

	for (int i = 0; i < original_path.size() - 1; i++)
	{
		for (int k = 0; k < lod; k++)
		{
			X = (P[i].x						*	B[k].x
				+ (P[i].x + d[i].x)	*	B[k].y
				+ (P[i + 1].x - d[i + 1].x)	*	B[k].z
				+ P[i + 1].x					*	B[k].w);

			Y = (P[i].y						*	B[k].x
				+ (P[i].y + d[i].y)		*	B[k].y
				+ (P[i + 1].y - d[i + 1].y)	*	B[k].z
				+ P[i + 1].y					*	B[k].w);

			Z = (P[i].z						*	B[k].x
				+ (P[i].z + d[i].z)		*	B[k].y
				+ (P[i + 1].z - d[i + 1].z)	*	B[k].z
				+ P[i + 1].z					*	B[k].w);

			if (i == 0 && k == 0)	
				result_path.push_back(vec3(X, Y, Z));
			if (k>0)				
				result_path.push_back(vec3(X, Y, Z));
			
		}
	}


	if (P != NULL)	delete[] P;
	if (d != NULL)	delete[] d;
	if (A != NULL)	delete[] A;
	if (B != NULL)	delete[] B;
	if (Bi)			delete[] Bi;

}

/*
vector<vec3> Spline(vector<vec3> points)
{
    int lod;
	if (points.size() < 3) return;
    vector3<vec3> result = {};
}
*/

/*
vec3 quadratic_bezier(vec3 (*interpolation)(vec3, vec3, float),
                      vec3 a, 
                      vec3 b, 
                      vec3 control, 
                      float t);

vec3 cubic_bezier(vec3 (*interpolation)(vec3, vec3, float), 
                  vec3 a, 
                  vec3 b, 
                  vec3 control1,
                  vec3 control2, 
                  float t);
*/


#endif
