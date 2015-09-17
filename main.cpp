#define STB_IMAGE_WRITE_IMPLEMENTATION
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#pragma clang diagnostic ignored "-Wcast-align"
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#include "stb_image_write.h"
#pragma clang diagnostic pop


union V3
{
	struct
	{
		float X, Y, Z;
	};
		struct
	{
		float R,G,B;
	};
	float Contents[3];
};


struct Sphere
{
	V3 Position;
	V3 Color;
	float Radius;
};

inline V3
operator*(float A, V3 B)
{
    V3 Result;

    Result.X = A*B.X;
    Result.Y = A*B.Y;
    Result.Z = A*B.Z;

    return(Result);
}

inline V3
operator*(V3 B, float A)
{
    V3 Result = A*B;

    return(Result);
}

inline V3 &
operator*=(V3 &B, float A)
{
    B = A * B;

    return(B);
}

inline V3
operator-(V3 A)
{
    V3 Result;

    Result.X = -A.X;
    Result.Y = -A.Y;
    Result.Z = -A.Z;

    return(Result);
}

inline V3
operator+(V3 A, V3 B)
{
    V3 Result;

    Result.X = A.X + B.X;
    Result.Y = A.Y + B.Y;
     Result.Z = A.Z + B.Z;

    return(Result);
}

inline V3 &
operator+=(V3 &A, V3 B)
{
    A = A + B;

    return(A);
}

inline V3
operator-(V3 A, V3 B)
{
    V3 Result;

    Result.X = A.X - B.X;
    Result.Y = A.Y - B.Y;
    Result.Z = A.Z - B.Z;

    return(Result);
}

inline float
dot(V3 A, V3 B)
{
    float Result = A.X*B.X + A.Y*B.Y+ A.Z*B.Z;

    return(Result);
}

inline double Length(V3 A)
{
	double Result = sqrt(dot(A,A));
	return Result;
}

inline V3 Normalise(V3 A)
{
	V3 Result = (1.0f/(float)Length(A))*A;
	return Result;
}


const int MAX_SPHERES = 32;
static Sphere Spheres[MAX_SPHERES];
static int NumberOfSpheres = 0;

inline void AddSphere(V3 Position, V3 Color, float Radius)
{
	if(NumberOfSpheres < MAX_SPHERES -1)
	{
		Spheres[NumberOfSpheres] . Position = Position;
		Spheres[NumberOfSpheres].Radius = Radius;
		Spheres[NumberOfSpheres++].Color = Color;
	}
}

int main(int, char **)
{


	AddSphere({{0,0,50}}, {{255,0,0}}, 20);
	AddSphere({{17,0,25}},{{0,0,255}}, 4);

	AddSphere({{-17,0,25}},{{0,255,255}}, 4);
	AddSphere({{0,0,0}},{0,0,100}, 100);

	int Width = 2048, Height = 1024, BytesPerPixel = 3;
	unsigned char * ImageData = (unsigned char*)malloc((size_t)(Width*Height*BytesPerPixel));

	V3 Origin = {{0,0,0}};
	for(int y=0;y<Height;y++)
	{
		for(int x=0;x<Width;x++)
		{
			V3 RayPosition = {{(x/(float)Width)*4.0f - 2.0f, -(y/(float)Height)*2.0f + 1.0f, 1}};
			V3 RayDirection = Normalise(RayPosition - Origin);
			V3 RayColor = {{0,0,0}};
			float intersection = 0;
			int Index = -1;
			int count = 0;
			float ColorCoeff = 1.0f;
			do
			{
				Index =-1;
				for(int i=0;i<NumberOfSpheres;i++)
				{
					V3 OC;
					OC=Spheres[i].Position-RayPosition;
					double lOC2=Length(OC)*Length(OC);
					double Radius2 = Spheres[i].Radius * Spheres[i].Radius;
					double len = 0;
					if (lOC2<Radius2)//starting inside sphere
					{

						double tca = dot(OC,RayDirection);
						double l2hc = Radius2 - lOC2  + tca*tca;
						len= (tca + sqrt(l2hc));
					}
					else //starting outside
					{
						double tca = dot(OC, RayDirection);
						if (tca < 0)
						{
							len =0;
						}
						else
						{


							double l2hc = Radius2 - lOC2 + tca*tca;
							len= l2hc > 0 ? tca - sqrt(l2hc) : 0.0;
						}
					}
					if(len > 0 && (len < intersection || Index ==-1))
					{
						intersection = len;
						Index = i;
					}
				}
				if(Index>=0)
				{

					V3 IntersectionPoint = RayPosition + RayDirection * intersection;
					V3 Normal = Normalise(IntersectionPoint - Spheres[Index].Position);
					float C = dot(Normal, RayDirection);
					RayColor = RayColor + ColorCoeff*(1.0f-C)*Spheres[Index].Color;
					ColorCoeff *= 0.5;

					RayDirection = Normalise(RayDirection - 2*C*Normal);
					RayPosition = IntersectionPoint + 0.1 * RayDirection;
					//	Index = -1;
				}


			}
			while(Index != -1 && ++count <20) ;
			ImageData[(x+y*Width)*BytesPerPixel +0 ] = RayColor.R;

			ImageData[(x+y*Width)*BytesPerPixel +1 ] = RayColor.G;
			ImageData[(x+y*Width)*BytesPerPixel +2 ] = RayColor.B;
		}
						printf("Line %d done\n",y);
	}

	 stbi_write_png("test.png", Width, Height, BytesPerPixel, ImageData,  Width * BytesPerPixel);

	return 0;

}
