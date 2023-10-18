//
// versions.c
//
// $Revision: 3 $
//
typedef struct versions_info
{
	char	*file_name;
	char	*version_str;
} versions_info_t;

static char	sound_versions_c_version[] = {"$Revision: 3 $"};

extern char	sound_sound_c_version[];

static versions_info_t	sound_module_versions[] = {
{"SOUND.C", sound_sound_c_version},
{"VERSIONS.C", sound_versions_c_version},

{0, 0}
};

versions_info_t *get_sound_module_versions(void)
{
	return(sound_module_versions);
}

char *get_sound_library_mode(void)
{
#ifdef DEBUG
	return("DEBUG");
#else
	return("RELEASE");
#endif
}

