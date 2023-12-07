#include "grid.h"

void main()
{
	grid g;
	g.get_grid("grid.txt");

	std::ofstream Out("out.txt");
	for (int i = 0; i < g.MeshXY.size(); i++)
	{
		Out << g.MeshXY[i].x << "\t" << g.MeshXY[i].y << std::endl;
	}
}