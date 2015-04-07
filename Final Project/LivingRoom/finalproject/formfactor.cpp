#include "stdafx.h"
#include "formfactor.h"

vector<float> formfactor(vector<Plane> SPL, Plane patch)
{
	vector<float> formfactorlist(SPL.size());
	Position3 O = Position3();

	for (int i = 0; i < SPL.size(); i++)
		formfactorlist[i] == 0;

	/* Calculate the center of this patch */
	for (int i = 0; i < 3; i++)
		O.pos[i] = (patch.position[0].pos[i] + patch.position[1].pos[i] + patch.position[2].pos[i] + patch.position[3].pos[i]) / 4;

	/* Construct the hemicube list */
	Hemicube HPL = Hemicube();
	HPL.setHemicube(patch, 1);

	vector<Plane>::iterator itr = HPL.patches.begin();
	while (itr != HPL.patches.end())
	{
		/* Calculate the center of this hemicube patch */
		Plane hp = *itr;
		Position3 P = Position3();
		for (int i = 0; i < 3; i++)
			P.pos[i] = (hp.position[0].pos[i] + hp.position[1].pos[i] + hp.position[2].pos[i] + hp.position[3].pos[i]) / 4;

		for (int i = 0; i < SPL.size(); i++)
		{
			Position3 P0, P1, P2, P3;
			P1 = SPL[i].position[0];
			P2 = SPL[i].position[1];
			P3 = SPL[i].position[2];

			float u, v, w, t;
			u = (P2.pos[1] - P1.pos[1])*(P3.pos[2] - P1.pos[2]) - (P2.pos[2] - P1.pos[2])*(P3.pos[1] - P1.pos[1]);
			v = (P2.pos[2] - P1.pos[2])*(P3.pos[0] - P1.pos[0]) - (P2.pos[0] - P1.pos[0])*(P3.pos[2] - P1.pos[2]);
			w = (P2.pos[0] - P1.pos[0])*(P3.pos[1] - P1.pos[1]) - (P2.pos[1] - P1.pos[1])*(P3.pos[0] - P1.pos[0]);
			t = ((P1.pos[0] - O.pos[0])*u + (P1.pos[1] - O.pos[1])*v + (P1.pos[2] - O.pos[2])*w) /
				((O.pos[0] - P.pos[0])*u + (O.pos[1] - P.pos[1])*v + (O.pos[2] - P.pos[2])*w);

			for (int j = 0; j < 3; j++)
				P0.pos[j] = O.pos[j] + t*(O.pos[j] - P.pos[j]);

			if (P0.pos[0] == O.pos[0] && P0.pos[1] == O.pos[1] && P0.pos[2] == O.pos[2]) continue;
			if (inPatch(SPL[i], P0)) 
				formfactorlist[i] += 0.25;
		}

		itr++;
	}

	return formfactorlist;
}

bool inPatch(Plane patch, Position3 point)
{
	/* Project the whole polygon onto x-y plane. If the projection is also a polygon,
	 * decide the clockwise order of the projection. If the projection is a line, 
	 * then project the polygon onto another plane and decide the clockwise order.
	 */

	/* Sort the vertices based on y coordinates */
	int top, bottom;
	int v0, v1, v2, v3;
	top = bottom = 0;
	for (int i = 0; i < 4; i++)
	{
		if (patch.position[i].pos[1]>patch.position[top].pos[1]) top = i;
		if (patch.position[i].pos[1] < patch.position[bottom].pos[1]) bottom = i;
	}

	/* Find the indices of the other two vertices */
	int v[2];
	int idx = 0;
	for (int i = 0; i < 4; i++)
	{
		if (i == top || i == bottom) continue;
		else
		{
			v[idx] = i;
			idx++;
		}
	}

	/* Calculate the d value of each line */
	float d_bt, d_v0b, d_v1b;
	d_bt = d(patch.position[top], patch.position[bottom], 0, 1);
	d_v0b = d(patch.position[bottom], patch.position[v[0]], 0, 1);
	d_v1b = d(patch.position[bottom], patch.position[v[1]], 0, 1);

/* x-z plane */
	if ((d_bt == d_v0b && d_bt == d_v1b) || (top == bottom) || (patch.position[0].pos[0]==patch.position[1].pos[0] && patch.position[0].pos[0]==patch.position[2].pos[0])) // The projection on x-y plane is a line
	{
		/* Project the polygon onto x-z plane */

		/* Sort the vertices based on z coordinates */
		for (int i = 0; i < 4; i++)
		{
			if (patch.position[i].pos[2]>patch.position[top].pos[2]) top = i;
			if (patch.position[i].pos[2] < patch.position[bottom].pos[2]) bottom = i;
		}

		/* Find the indices of the other two vertices */
		idx = 0;
		for (int i = 0; i < 4; i++)
		{
			if (i == top || i == bottom) continue;
			else
			{
				v[idx] = i;
				idx++;
			}
		}

		d_bt = d(patch.position[top], patch.position[bottom], 0, 2);
		d_v0b = d(patch.position[bottom], patch.position[v[0]], 0, 2);
		d_v1b = d(patch.position[bottom], patch.position[v[1]], 0, 2);

/* y-z plane */
		if ((d_bt == d_v0b && d_bt == d_v1b) || (top == bottom) || (patch.position[0].pos[0] == patch.position[1].pos[0] && patch.position[0].pos[0] == patch.position[2].pos[0])) // The projection on x-z plane is also a line
		{

			/* Decide right/left of the two vertices */
			float x0, x1;
			x0 = patch.position[top].pos[1] - (patch.position[top].pos[1] - patch.position[bottom].pos[1]) / (patch.position[top].pos[2] - patch.position[bottom].pos[2])
				*(patch.position[top].pos[2] - patch.position[v[1]].pos[2]);
			x1 = patch.position[top].pos[1] - (patch.position[top].pos[1] - patch.position[bottom].pos[1]) / (patch.position[top].pos[2] - patch.position[bottom].pos[2])
				*(patch.position[top].pos[2] - patch.position[v[1]].pos[2]);

			v0 = top;
			if ((x0 - patch.position[v[0]].pos[1])*(x1 - patch.position[v[1]].pos[1]) < 0)
			{
				v0 = top;
				v2 = bottom;
				if (patch.position[v[0]].pos[1] > x0)
				{
					v1 = v[0];
					v3 = v[1];
				}
				else
				{
					v1 = v[1];
					v3 = v[0];
				}
			}
			else if (patch.position[v[0]].pos[1] > x0 && patch.position[v[1]].pos[1] > x1)
			{
				v0 = top;
				v3 = bottom;
				if (patch.position[v[0]].pos[2] > patch.position[v[1]].pos[2])
				{
					v1 = v[0];
					v2 = v[1];
				}
				else
				{
					v1 = v[1];
					v2 = v[0];
				}
			}
			else if (patch.position[v[0]].pos[1] < x0 && patch.position[v[1]].pos[1] < x1)
			{
				v0 = top;
				v1 = bottom;
				if (patch.position[v[0]].pos[2] > patch.position[v[1]].pos[2])
				{
					v3 = v[0];
					v2 = v[1];
				}
				else
				{
					v3 = v[1];
					v2 = v[0];
				}
			}

			if (LEE(patch.position[v1], patch.position[v0], point, 1, 2) >= 0 &&
				LEE(patch.position[v2], patch.position[v1], point, 1, 2) >= 0 &&
				LEE(patch.position[v3], patch.position[v2], point, 1, 2) >= 0 &&
				LEE(patch.position[v0], patch.position[v3], point, 1, 2) >= 0)
				return true;

			else return false;
		}

		/* Decide right/left of the two vertices */
		float x0, x1;
		x0 = patch.position[top].pos[0] - (patch.position[top].pos[0] - patch.position[bottom].pos[0]) / (patch.position[top].pos[2] - patch.position[bottom].pos[2])
			*(patch.position[top].pos[2] - patch.position[v[0]].pos[2]);
		x1 = patch.position[top].pos[0] - (patch.position[top].pos[0] - patch.position[bottom].pos[0]) / (patch.position[top].pos[2] - patch.position[bottom].pos[2])
			*(patch.position[top].pos[2] - patch.position[v[1]].pos[2]);

		v0 = top;
		if ((x0 - patch.position[v[0]].pos[0])*(x1 - patch.position[v[1]].pos[0]) < 0)
		{
			v0 = top;
			v2 = bottom;
			if (patch.position[v[0]].pos[0] > x0)
			{
				v1 = v[0];
				v3 = v[1];
			}
			else
			{
				v1 = v[1];
				v3 = v[0];
			}
		}
		else if (patch.position[v[0]].pos[0] > x0 && patch.position[v[1]].pos[0] > x1)
		{
			v0 = top;
			v3 = bottom;
			if (patch.position[v[0]].pos[2] > patch.position[v[1]].pos[2])
			{
				v1 = v[0];
				v2 = v[1];
			}
			else
			{
				v1 = v[1];
				v2 = v[0];
			}
		}
		else if (patch.position[v[0]].pos[0] < x0 && patch.position[v[1]].pos[0] < x1)
		{
			v0 = top;
			v1 = bottom;
			if (patch.position[v[0]].pos[2] > patch.position[v[1]].pos[2])
			{
				v3 = v[0];
				v2 = v[1];
			}
			else
			{
				v3 = v[1];
				v2 = v[0];
			}
		}

		if (LEE(patch.position[v1], patch.position[v0], point, 0, 2) >= 0 &&
			LEE(patch.position[v2], patch.position[v1], point, 0, 2) >= 0 &&
			LEE(patch.position[v3], patch.position[v2], point, 0, 2) >= 0 &&
			LEE(patch.position[v0], patch.position[v3], point, 0, 2) >= 0)
			return true;

	}
	else
	{
		/* Decide right/left of the two vertices */
		float x0, x1;
		x0 = patch.position[top].pos[0] - (patch.position[top].pos[0] - patch.position[bottom].pos[0]) / (patch.position[top].pos[1] - patch.position[bottom].pos[1])
			*(patch.position[top].pos[1] - patch.position[v[0]].pos[1]);
		x1 = patch.position[top].pos[0] - (patch.position[top].pos[0] - patch.position[bottom].pos[0]) / (patch.position[top].pos[1] - patch.position[bottom].pos[1])
			*(patch.position[top].pos[1] - patch.position[v[1]].pos[1]);

		v0 = top;
		if ((x0 - patch.position[v[0]].pos[0])*(x1 - patch.position[v[1]].pos[0]) < 0)
		{
			v0 = top;
			v2 = bottom;
			if (patch.position[v[0]].pos[0] > x0)
			{
				v1 = v[0];
				v3 = v[1];
			}
			else
			{
				v1 = v[1];
				v3 = v[0];
			}
		}
		else if (patch.position[v[0]].pos[0] > x0 && patch.position[v[1]].pos[0] > x1)
		{
			v0 = top;
			v3 = bottom;
			if (patch.position[v[0]].pos[1] > patch.position[v[1]].pos[1])
			{
				v1 = v[0];
				v2 = v[1];
			}
			else
			{
				v1 = v[1];
				v2 = v[0];
			}
		}
		else if (patch.position[v[0]].pos[0] < x0 && patch.position[v[1]].pos[0] < x1)
		{
			v0 = top;
			v1 = bottom;
			if (patch.position[v[0]].pos[1] > patch.position[v[1]].pos[1])
			{
				v3 = v[0];
				v2 = v[1];
			}
			else
			{
				v3 = v[1];
				v2 = v[0];
			}
		}

		if (LEE(patch.position[v1], patch.position[v0], point, 0, 1) > 0 &&
			LEE(patch.position[v2], patch.position[v1], point, 0, 1) > 0 &&
			LEE(patch.position[v3], patch.position[v2], point, 0, 1) > 0 &&
			LEE(patch.position[v0], patch.position[v3], point, 0, 1) > 0)
			return true;
	}
	
	return false;
}

float d(Position3 u, Position3 v, int i, int j)
{
	return (u.pos[i] - v.pos[i]) / (u.pos[j] - v.pos[j]);
}

float LEE(Position3 head, Position3 tail, Position3 point, int i, int j)
{
	float deltaX = head.pos[i] - tail.pos[i];
	float deltaY = head.pos[j] - tail.pos[j];
	float x = point.pos[i];
	float y = point.pos[j];

	float decision = deltaY*x - deltaX*y + deltaX*tail.pos[j] - deltaY*tail.pos[i];
	return decision;
}