#include <iostream>
#include <vector>
#include <string>
#include <raylib.h>
#include <math.h>

using decimal = float;

struct body {
  decimal px,py,pz,vx,vy,vz;
  decimal mass;
  decimal radius;
};

decimal randfl() {
  return ((decimal)(GetRandomValue(0,1000)))*0.001;
}

decimal radius_mass_ratio = 0.1;

decimal radius(decimal mass) {
  return radius_mass_ratio*pow(mass,1.0/3.0);
}

int main() {

  std::vector<body> bodies;

  SetRandomSeed(0x9879CA2F);
  
  int num_bodies = 10;
  int width = 800;
  int height = 800;
  decimal g = 1e-4;
  decimal dt = 0.001;
  
  for (int i=0;i<num_bodies;i++) {
    bodies.push_back(
      {randfl(), randfl(), randfl(),
      (randfl()-0.5)*0.02, (randfl()-0.5)*0.02, (randfl()-0.5)*0.02,
      1.0,
      radius(1.0)});
  }
  InitWindow(width,height, "test1");  
  SetTargetFPS(60);

  int computes_per_frame = 100;

  Camera3D camera={0};
  camera.position=(Vector3){2.0f,2.0f,2.0f};
  camera.target=(Vector3){0.0f,0.0f,0.0f};
  camera.up=(Vector3){0.0f,0.0f,1.0f};
  camera.fovy = 30.f;
  camera.projection = CAMERA_PERSPECTIVE;

  Image img = GenImageColor(32,32, BLANK);

  float lightDirX = 2.0;
  float lightDirY = 1.0;
  float lightDirZ = 0.5;

  float lightDirLen = sqrt(lightDirX*lightDirX+lightDirY*lightDirY+lightDirZ*lightDirZ);
  lightDirX/= lightDirLen;
  lightDirY/= lightDirLen;
  lightDirZ/= lightDirLen;

  for (int i=0;i<32;i++) {
    for (int j=0;j<32;j++) {
      int dist = (i-16)*(i-16) + (j-16)*(j-16);
      if (dist < 256) {
        float x = ((float)(i-16))/16;
        float y = ((float)(j-16))/16;
        float phi = sqrt(x*x+y*y);
        float theta = atan2(y,x);
        float nx = cos(theta)*sin(phi);
        float ny = sin(theta)*sin(phi);
        float nz = cos(phi);
        float dot = lightDirX*nx + lightDirY*ny + lightDirZ*nz;
        ImageDrawPixel(&img, i,j, (Color){255*dot,255*dot,255*dot,255});
      }
    }
  }
  
  while (!WindowShouldClose()) {
    ClearBackground(BLACK);
    
    BeginDrawing();
    BeginMode3D(camera);
    for (auto &b : bodies) {
      DrawSphere(Vector3(b.px,b.py,b.pz), b.radius, WHITE);
    }
    EndMode3D();
    DrawFPS(0,0);

    EndDrawing();
    
    for (int k=0;k<computes_per_frame;k++) {
      // gravitation
      for (int i=0;i<bodies.size();i++) {
        decimal ax = 0.f;
        decimal ay = 0.f;
        decimal az = 0.f;
        auto &b1 = bodies[i];
        for (int j=0;j<bodies.size();j++) {
          auto &b2 = bodies[j];
          if (i==j) continue;
          decimal dist2 = b1.px*b2.px + b1.py*b2.py + b1.pz*b2.pz;
          decimal dist = sqrt(dist2);
          ax += (b2.px-b1.px)/dist/dist2;
          ay += (b2.py-b1.py)/dist/dist2;
          az += (b2.pz-b1.pz)/dist/dist2;
        }
        b1.vx += dt*g*ax;
        b1.vy += dt*g*ay;
        b1.vz += dt*g*az;
      }
      // integration
      for (auto &b : bodies) {
        b.px += dt*b.vx;
        b.py += dt*b.vy;
        b.pz += dt*b.vz;
      }
    }
  }
  CloseWindow();
  
  return 0;
}
