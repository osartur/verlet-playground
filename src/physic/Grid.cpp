#include "Grid.h"
#include "Ball.h"
#include <cmath>

Grid::Grid()
{
	
}

Grid::~Grid()
{
	
}

void Grid::config(const vec& size, const vec& quadDimension)
{
	gridSize = size;
	gridPos = vec(0.f, 0.f);
	quadSize = quadDimension;
	rowCount = ceil(gridSize.y / quadSize.y) + 1;
	colCount = ceil(gridSize.x / quadSize.x) + 1;
	quads.resize(rowCount * colCount);
}

void Grid::loadObjects(std::vector<Ball>& objects)
{
	for (Quad& q : quads)
	{
		q.clear();
	}
	
	for (Ball& b : objects)
	{
		Quad* cell = quad(b.pos);
		if (cell != nullptr)
		{
			cell->push_back(&b);
		}
	}
}

void Grid::detectCollisions()
{
    for (int i = 0; i < quads.size(); i++)
    {
        int row = i / colCount;
        int col = i % colCount;
        neighborCellsCache.clear();
        loadAdjacentCells(row, col, neighborCellsCache);
        Quad* currentCell = quad(row, col);
        for (Ball* object : *currentCell)
        {
            collide(object, neighborCellsCache);
        }
    }
}

Ball* Grid::findObject(const vec& point)
{
    int row = point.y / quadSize.y;
	int col = point.x / quadSize.x;
    neighborCellsCache.clear();
    loadAdjacentCells(row, col, neighborCellsCache);
    for (Quad* cell : neighborCellsCache)
    {
        for (Ball* obj : *cell)
        {
            if (hasCollision(*obj, point))
            {
                return obj;
            }
        }
    }
    return nullptr;
}

void Grid::loadAdjacentCells(int row, int col, std::vector<Quad*>& dest)
{
    int leftQuad = col == 0 ? col : col - 1;
    int rightQuad = col + 1 == colCount ? col : col + 1;
    int topQuad = row == 0 ? row : row - 1;
    int bottomQuad = row + 1 == rowCount ? row : row + 1;
    
    for (int i = topQuad; i < bottomQuad; i++)
    {
        for (int j = leftQuad; j < rightQuad; j++)
        {
            dest.push_back(quad(i, j));
        }
    }
}

void Grid::collide(Ball* target, std::vector<Quad*>& neighbors)
{
	for (Quad* cell : neighbors)
	{
	    for (Ball* obj : *cell)
	    {
	        collide(target, obj);
	    }
	}
}

void Grid::collide(Ball* b1, Ball* b2)
{
    if (b1 == b2 || !hasCollision(*b1, *b2))
    {
        return;
    }
    float minDist = b1->radius + b2->radius;
	setDistance(b1, b2, minDist);
}

bool Grid::hasCollision(const Ball& b1, const Ball& b2)
{
    vec axis = b1.pos - b2.pos;
    float dist2 = axis.x * axis.x + axis.y * axis.y;
    float minDist = b1.radius + b2.radius;
    return dist2 < minDist * minDist;
}

bool Grid::hasCollision(const Ball& ball, const vec& pos)
{
    const float radiusDelta = 16.f;
    vec axis = ball.pos - pos;
    float dist2 = axis.x * axis.x + axis.y * axis.y;
    float minDist = ball.radius + radiusDelta;
    return dist2 < minDist * minDist;
}

Grid::Quad* Grid::quad(int row, int col)
{
	if (row < 0 || row >= rowCount)
	{
		return nullptr;
	}
	if (col < 0 || col >= colCount)
	{
		return nullptr;
	}
	int index = row * colCount + col;
	return &quads[index];
}

Grid::Quad* Grid::quad(const vec& point)
{
	if (point.x < gridPos.x || point.x > gridPos.x + gridSize.x)
	{
		return nullptr;
	}
	if (point.y < gridPos.y || point.y > gridPos.y + gridSize.y)
	{
		return nullptr;
	}
	int row = point.y / quadSize.y;
	int col = point.x / quadSize.x;
	int index = row * colCount + col;
	return &quads[index];
}
