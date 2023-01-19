#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
//#include <GL/gl.h>

#include <vector>

#ifndef VERTEX_H
#define VERTEX_H
struct vertex
{
        glm::vec3       position;
        glm::vec3       normal;
        glm::vec3       color;
        glm::vec2       texCoord;
};
#endif


// delimiters for parsing the obj file:

#define OBJDELIMS		" \t"


struct Vertex
{
	float x, y, z;
};


struct Normal
{
	float nx, ny, nz;
};


struct TextureCoord
{
	float s, t, p;
};


struct face
{
	int v, n, t;
};



void	Cross( float [3], float [3], float [3] );
char *	ReadRestOfLine( FILE * );
void	ReadObjVTN( char *, int *, int *, int * );
float	Unit( float [3] );
float	Unit( float [3], float [3] );


MyBuffer
VkuLoadObjFile( char *name )
{
	char *cmd;		// the command string
	char *str;		// argument string

	std::vector <struct Vertex> Vertices(10000);
	std::vector <struct Normal> Normals(10000);
	std::vector <struct TextureCoord> TextureCoords(10000);

	Vertices.clear();
	Normals.clear();
	TextureCoords.clear();

	struct Vertex sv;
	struct Normal sn;
	struct TextureCoord st;
	MyBuffer		myVertexDataBuffer;


	// open the input file:

	FILE *fp = fopen( name, "r" );
	if( fp == NULL )
	{
		fprintf( stderr, "Cannot open .obj file '%s'\n", name );
		myVertexDataBuffer.size = 0;
		return myVertexDataBuffer;
	}



	int numTriangles = 0;
	int numVertices = 0;

	float xmin = 1.e+37f;
	float ymin = 1.e+37f;
	float zmin = 1.e+37f;
	float xmax = -xmin;
	float ymax = -ymin;
	float zmax = -zmin;

	for( ; ; )
	{
		char *line = ReadRestOfLine( fp );
		if( line == NULL )
			break;

		// skip this line if it is not a vertex or face:

		cmd = strtok( line, OBJDELIMS );

		if (strcmp(cmd, "v") == 0)
		{
			str = strtok(NULL, OBJDELIMS);
			float x = (float)atof(str);

			str = strtok(NULL, OBJDELIMS);
			float y = (float)atof(str);

			str = strtok(NULL, OBJDELIMS);
			float z = (float)atof(str);

			if (x < xmin)	xmin = x;
			if (x > xmax)	xmax = x;
			if (y < ymin)	ymin = y;
			if (y > ymax)	ymax = y;
			if (z < zmin)	zmin = z;
			if (z > zmax)	zmax = z;

			continue;
		}
			
		if (strcmp(cmd,"f") != 0 )
			continue;


		struct face vertices[10];
		for( int i = 0; i < 10; i++ )
		{
			vertices[i].v = 0;
			vertices[i].n = 0;
			vertices[i].t = 0;
		}

		int sizev = (int)Vertices.size();
		int sizen = (int)Normals.size();
		int sizet = (int)TextureCoords.size();

		bool valid = true;
		int vtx = 0;
		char *str;
		while( ( str = strtok( NULL, OBJDELIMS ) )  !=  NULL )
		{
			int v, n, t;
			ReadObjVTN( str, &v, &t, &n );

			vtx++;

			if( vtx >= 10 )
				break;

			numVertices++;
		}

	}

	numTriangles = numVertices/3;

	float xcenter = (xmin + xmax) / 2.f;
	float ycenter = (ymin + ymax) / 2.f;
	float zcenter = (zmin + zmax) / 2.f;



	// **********************************
	// allocate the correct sized vulkan buffer for an array of struct vertex [numVertices]
	// **********************************

	// # triangles x 3 vertices/triangle:
    Init05MyVertexDataBuffer(  numTriangles*3*sizeof(struct vertex), OUT &myVertexDataBuffer );

    struct vertex * pVertex;
    vkMapMemory( LogicalDevice, IN myVertexDataBuffer.vdm, 0, VK_WHOLE_SIZE, 0, (void **)&pVertex );



	rewind( fp );

	for( ; ; )
	{
		char *line = ReadRestOfLine( fp );
		if( line == NULL )
			break;


		// skip this line if it is a comment:

		if( line[0] == '#' )
			continue;


		// skip this line if it is something we don't feel like handling today:

		if( line[0] == 'g' )
			continue;

		if( line[0] == 'm' )
			continue;

		if( line[0] == 's' )
			continue;

		if( line[0] == 'u' )
			continue;


		// get the command string:

		cmd = strtok( line, OBJDELIMS );


		// skip this line if it is empty:

		if( cmd == NULL )
			continue;


		if( strcmp( cmd, "v" )  ==  0 )
		{
			str = strtok( NULL, OBJDELIMS );
			sv.x = (float)atof(str) - xcenter;

			str = strtok( NULL, OBJDELIMS );
			sv.y = (float)atof(str) - ycenter;

			str = strtok( NULL, OBJDELIMS );
			sv.z = (float)atof(str) - zcenter;

			Vertices.push_back( sv );

			continue;
		}


		if( strcmp( cmd, "vn" )  ==  0 )
		{
			str = strtok( NULL, OBJDELIMS );
			sn.nx = (float)atof( str );

			str = strtok( NULL, OBJDELIMS );
			sn.ny = (float)atof( str );

			str = strtok( NULL, OBJDELIMS );
			sn.nz = (float)atof( str );

			Normals.push_back( sn );

			continue;
		}


		if( strcmp( cmd, "vt" )  ==  0 )
		{
			st.s = st.t = st.p = 0.;

			str = strtok( NULL, OBJDELIMS );
			st.s = (float)atof( str );

			str = strtok( NULL, OBJDELIMS );
			if( str != NULL )
				st.t = (float)atof( str );

			str = strtok( NULL, OBJDELIMS );
			if( str != NULL )
				st.p = (float)atof( str );

			TextureCoords.push_back( st );

			continue;
		}


		if( strcmp( cmd, "f" )  ==  0 )
		{
			struct face vertices[10];
			for( int i = 0; i < 10; i++ )
			{
				vertices[i].v = 0;
				vertices[i].n = 0;
				vertices[i].t = 0;
			}

			int sizev = (int)Vertices.size();
			int sizen = (int)Normals.size();
			int sizet = (int)TextureCoords.size();

			int numVertices = 0;
			bool valid = true;
			int vtx = 0;
			char *str;
			while( ( str = strtok( NULL, OBJDELIMS ) )  !=  NULL )
			{
				int v, n, t;
				ReadObjVTN( str, &v, &t, &n );

				// if v, n, or t are negative, they are wrt the end of their respective list:

				if( v < 0 )
					v += ( sizev + 1 );

				if( n < 0 )
					n += ( sizen + 1 );

				if( t < 0 )
					t += ( sizet + 1 );


				// be sure we are not out-of-bounds (<vector> will abort):

				if( t > sizet )
				{
					if( t != 0 )
						fprintf( stderr, "Read texture coord %d, but only have %d so far\n", t, sizet );
					t = 0;
				}

				if( n > sizen )
				{
					if( n != 0 )
						fprintf( stderr, "Read normal %d, but only have %d so far\n", n, sizen );
					n = 0;
				}

				if( v > sizev )
				{
					if( v != 0 )
						fprintf( stderr, "Read vertex coord %d, but only have %d so far\n", v, sizev );
					v = 0;
					valid = false;
				}

				vertices[vtx].v = v;
				vertices[vtx].n = n;
				vertices[vtx].t = t;
				vtx++;

				if( vtx >= 10 )
					break;

				numVertices++;
			}


			// if vertices are invalid, don't draw anything this time:

			if( ! valid )
				continue;

			if( numVertices < 3 )
				continue;


			// list the vertices:

			int numTriangles = numVertices - 2;

			for( int it = 0; it < numTriangles; it++ )
			{
				int vv[3];
				vv[0] = 0;
				vv[1] = it + 1;
				vv[2] = it + 2;

				// get the planar normal, in case vertex normals are not defined:

				struct Vertex *v0 = &Vertices[ vertices[ vv[0] ].v - 1 ];
				struct Vertex *v1 = &Vertices[ vertices[ vv[1] ].v - 1 ];
				struct Vertex *v2 = &Vertices[ vertices[ vv[2] ].v - 1 ];

				float v01[3], v02[3], norm[3];
				v01[0] = v1->x - v0->x;
				v01[1] = v1->y - v0->y;
				v01[2] = v1->z - v0->z;
				v02[0] = v2->x - v0->x;
				v02[1] = v2->y - v0->y;
				v02[2] = v2->z - v0->z;
				Cross( v01, v02, norm );
				Unit( norm, norm );
				//glNormal3fv( norm );
				pVertex->normal = glm::vec3( norm[0], norm[1], norm[2] );

				for( int vtx = 0; vtx < 3 ; vtx++ )
				{
					if( vertices[ vv[vtx] ].t != 0 )
					{
						struct TextureCoord *tp = &TextureCoords[ vertices[ vv[vtx] ].t - 1 ];
						//glTexCoord2f( tp->s, tp->t );
						pVertex->texCoord = glm::vec2( tp->s, tp->t );
					}

					if( vertices[ vv[vtx] ].n != 0 )
					{
						struct Normal *np = &Normals[ vertices[ vv[vtx] ].n - 1 ];
						//glNormal3f( np->nx, np->ny, np->nz );
						pVertex->normal = glm::vec3( np->nx, np->ny, np->nz );
					}

					struct Vertex *vp = &Vertices[ vertices[ vv[vtx] ].v - 1 ];
					//glVertex3f( vp->x, vp->y, vp->z );
					pVertex->position = glm::vec3( vp->x, vp->y, vp->z );
					pVertex->color = glm::vec3( 0., 1., 0. );
					pVertex++;
				}
			}
			continue;
		}


		if( strcmp( cmd, "s" )  ==  0 )
		{
			continue;
		}

	}

	fclose( fp );

	fprintf(stderr, "OBJ file: '%s'\n", name);
	fprintf(stderr, "\t# Triangles = %d , # Vertices = %d\n", numTriangles, numVertices);
	fprintf(stderr, "\tOriginal object center: (%8.3f,%8.3f,%8.3f)\n", xcenter, ycenter, zcenter);
	fprintf(stderr, "\tOriginal object range: [%8.3f,%8.3f,%8.3f] -> [%8.3f,%8.3f,%8.3f]\n",
		xmin, ymin, zmin,  xmax, ymax, zmax );
	fprintf(stderr, "\tObject center has been repositioned to (%8.3f,%8.3f,%8.3f)\n", 0., 0., 0.);
	fprintf(stderr, "\tNew Object range: [%8.3f,%8.3f,%8.3f] -> [%8.3f,%8.3f,%8.3f]\n",
		xmin-xcenter, ymin-ycenter, zmin-zcenter, xmax-xcenter, ymax-ycenter, zmax-zcenter);
	fprintf( stderr, "\tObject span: (%8.3f,%8.3f,%8.3f)\n",
		xmax-xmin, ymax-ymin, zmax-zmin );

        vkUnmapMemory( LogicalDevice, IN myVertexDataBuffer.vdm );
	return myVertexDataBuffer;
}



void
Cross( float v1[3], float v2[3], float vout[3] )
{
	float tmp[3];

	tmp[0] = v1[1]*v2[2] - v2[1]*v1[2];
	tmp[1] = v2[0]*v1[2] - v1[0]*v2[2];
	tmp[2] = v1[0]*v2[1] - v2[0]*v1[1];

	vout[0] = tmp[0];
	vout[1] = tmp[1];
	vout[2] = tmp[2];
}



float
Unit( float v[3] )
{
	float dist;

	dist = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];

	if( dist > 0.0 )
	{
		dist = sqrt( dist );
		v[0] /= dist;
		v[1] /= dist;
		v[2] /= dist;
	}

	return dist;
}



float
Unit( float vin[3], float vout[3] )
{
	float dist;

	dist = vin[0]*vin[0] + vin[1]*vin[1] + vin[2]*vin[2];

	if( dist > 0.0 )
	{
		dist = sqrt( dist );
		vout[0] = vin[0] / dist;
		vout[1] = vin[1] / dist;
		vout[2] = vin[2] / dist;
	}
	else
	{
		vout[0] = vin[0];
		vout[1] = vin[1];
		vout[2] = vin[2];
	}

	return dist;
}


char *
ReadRestOfLine( FILE *fp )
{
	static char *line;
	std::vector<char> tmp(1000);
	tmp.clear();

	for( ; ; )
	{
		int c = getc( fp );

		if( c == EOF  &&  tmp.size() == 0 )
		{
			return NULL;
		}

		if( c == EOF  ||  c == '\n' )
		{
			delete [] line;
			line = new char [ tmp.size()+1 ];
			for( int i = 0; i < (int)tmp.size(); i++ )
			{
				line[i] = tmp[i];
			}
			line[ tmp.size() ] = '\0';	// terminating null
			return line;
		}
		else
		{
			tmp.push_back( c );
		}
	}

	return "";
}


void
ReadObjVTN( char *str, int *v, int *t, int *n )
{
	// can be one of v, v//n, v/t, v/t/n:

	if( strstr( str, "//") )				// v//n
	{
		*t = 0;
		sscanf( str, "%d//%d", v, n );
		return;
	}
	else if( sscanf( str, "%d/%d/%d", v, t, n ) == 3 )	// v/t/n
	{
		return;
	}
	else
	{
		*n = 0;
		if( sscanf( str, "%d/%d", v, t ) == 2 )		// v/t
		{
			return;
		}
		else						// v
		{
			*n = *t = 0;
			sscanf( str, "%d", v );
		}
	}
}
