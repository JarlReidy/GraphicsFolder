#pragma once
#include <vector>
#include <set>

class StaticCollider;

class GridCell
{
	std::vector<StaticCollider*> colliders;
public:
	GridCell() {}
	void Clear() { colliders.clear(); }
	void AddCollider(StaticCollider* c)
	{
		colliders.push_back(c);
	}
	std::vector<StaticCollider*> GetColliders() const { return colliders; }
};

class CollisionGrid
{
	int nx, nz;
	float delta;
	float x0, z0; // origin of grid
	std::vector<std::vector<GridCell>> grid;
public:
	CollisionGrid(float xMin, float xMax, float zMin, float zMax, float delta) : delta(delta), x0(xMin), z0(zMin)
	{
		nx = static_cast<int>((xMax - xMin) / delta);
		nz = static_cast<int>((zMax - zMin) / delta);

		for (int i = 0; i < nx; i++)
		{
			grid.push_back(std::vector<GridCell>(nz, GridCell()));
		}
	}
	void AddCollider(StaticCollider* c, float xmin, float xmax, float zmin, float zmax)
	{
		int i0 = (int)((xmin - x0) / delta);
		int i1 = (int)((xmax - x0) / delta);
		int j0 = (int)((zmin - z0) / delta);
		int j1 = (int)((zmax - z0) / delta);
		i0 = std::max(i0, 0);
		j0 = std::max(j0, 0);
		i1 = std::min(i1, nx - 1);
		j1 = std::min(j1, nz - 1);
		for (int i = i0; i <= i1; i++)
		{
			for (int j = j0; j <= j1; j++)
			{
				grid[i][j].AddCollider(c);
			}
		}
	}
	std::set<StaticCollider*> GetColliders(float xmin, float xmax, float zmin, float zmax) const
	{
		std::set<StaticCollider*> colliders;

		return colliders;
	}
};
