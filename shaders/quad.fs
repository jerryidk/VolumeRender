#version 410 core

out vec4 fcolor;

in vec3 TexCoord;

uniform sampler3D volumeTex;

//must be in projection space.
uniform vec3 cameraPosInSTR;

//Between 0.0 and 1.0
uniform float isovalue;
//Step advanced for each ray, keep this relatively small, gets costly
uniform float stepsize;

//Cube color
uniform vec3 background; 

int backtofront = 0;

int tfType = 0;
const float PI = 3.1415926535;

float mu = 0.5;
float sigma = 0.55;

vec4 TF(float scalar)
{
   if(tfType == 0)
      return vec4(vec3(scalar), scalar*0.5);
   
   if(tfType == 1)
   {
      float c = pow(scalar-mu, 2);
      float c2 = 2 * pow(sigma, 2);
      float num = exp(-c/c2); 
      float den = pow(2*PI, 0.5) * sigma;
      scalar = num/den;
      return vec4(scalar);
   }
}

// See Nvidia GPU gem3 volume rendering. 
vec4 blending(vec4 src, vec4 dst)
{
   if(backtofront == 1)
   {
      //back to front src over dst
      dst = src * src.a + (1.0 - src.a) * dst;
   }
   else
   {
      //front to back src under dst
      dst.rgb += (1 - dst.a) * src.rgb * src.a;
      dst.a   += (1 - dst.a) * src.a;
   }

   return dst;
}

void main()
{
   //background color
   vec4 dst = vec4(background, 0.0);
   //Iteration of loops, # of ray can maximum traverse (upper bound), iter > stepsize
   int iter = 500;

   vec3 rayDir = normalize(TexCoord - cameraPosInSTR);
   vec3 step = rayDir / stepsize;

   //This the intersection point in st/uv space 
   vec3 pos = TexCoord;

   //dst = texture(volumeTex, pos).r;
   vec4 src = vec4(0.0);
   //ray casting algorithm
   for(int i = 0; i < iter; i++)
   {
      float scalar = texture(volumeTex, pos).r;

      if(scalar < isovalue)
         src = vec4(background, 0.0);
      else
         src = TF(scalar);

      dst = blending(src, dst);
      
      if(dst.a >= .95f)
            break; 
      //advance ray
      pos += step; 
      //check if pos out of bound
      if(pos.x > 1.0 || pos.y > 1.0 || pos.z > 1.0 
         || pos.x < 0.0 || pos.y < 0.0 || pos.z < 0.0)
         {
            break;
         }
   }

   //Output the color
   //gamma correction
   fcolor = pow(dst, vec4(1/2.2));
}