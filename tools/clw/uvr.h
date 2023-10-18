#define MATCH				0.0001f
#define MAX_TEXTURES		128
#define TEXTURE_LEN			32
#define MAX_TRIANGLES		4096

typedef struct _vertex_t {
	float u,v;
} vertex_t;

int read_uv( char * );