#include <iostream>
#include <SDL.h>
using namespace std;

int main(int argc,char*argv[]){

    SDL_Window* window = NULL;      //declare a pointer
    SDL_Renderer* renderer = NULL;

    SDL_Init(SDL_INIT_VIDEO);         //Initializa SDL2

    window = SDL_CreateWindow(
      "the first window",           //window title
      100,                          //initial x poision
      100,                          //initial y poision    
      524,                          //width, in pixels
      568,                          //width, in pixels
      SDL_WINDOW_OPENGL             //flags - see below
  );        


    if (window == NULL) {
  
      cout << "Could not create a window" << SDL_GetError() << endl;
      return 1;
     }

   int quit =1;
    renderer = SDL_CreateRenderer(window, -1, 0);
    
    if (!renderer) {
              return -1;
      
       
     }
      
     SDL_SetRenderDrawColor(renderer, 255, 192, 203, 255); 
     SDL_RenderClear(renderer);
     SDL_RenderPresent(renderer); 

     while (quit) {
          SDL_Event event;
          SDL_WaitEvent(&event);
          switch (event.type) {
          case SDL_QUIT:
             SDL_Log("quit");
              quit = 0;
              break;
          default:
              SDL_Log("event type:%d", event.type);
          }
      }

      SDL_DestroyRenderer(renderer); 
 
  
  //SDL_Delay(3000);

  //clean and destroy the window
  SDL_DestroyWindow(window);

  //clean up
  SDL_Quit();

  return 0;
}