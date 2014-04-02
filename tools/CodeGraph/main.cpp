#pragma comment(lib, "cgraph.lib")
#pragma comment(lib, "gvc.lib")

#include <gvc.h>
#include <stdio.h>
#include <iostream>

int main(int argc, char **argv)
{
   GVC_t *gvc = gvContext();

   FILE *fh;
   fopen_s(&fh, "test.dot", "r");
   Agraph_t *graph = agread(fh, 0);
   int ret = gvLayout(gvc, graph, "dot");

   for (Agnode_t* node = agfstnode(graph); node != NULL; node = agnxtnode(graph, node)) {
      auto coord = ND_coord(node);
      auto width = ND_width(node);
      auto height = ND_height(node);

      std::cout << "Node " << ND_id(node) << " [" << coord.x << ", " << coord.y << "] [" << width << "x" << height << "]" << std::endl;

      for (Agedge_t* edge = agfstout(graph, node); edge != NULL; edge = agnxtout(graph, edge)) {
         auto splines = ED_spl(edge);
         
         std::cout << "Edge splines " << splines->list->size << std::endl;
      }
   }

   fclose(fh);
   return 0;
}
