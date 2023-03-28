#ifndef _NETCDF_API_H
#define _NETCDF_API_H


// netcdf_get_variables : récupère les variables du fichier donné et met à jour la combo box
void netcdf_get_variables(const char *filename);

// netcdf_get_dim : extrait les dimensions des axes et la date
void netcdf_get_metadata(const char *filename, const char *var, size_t *x_axis, size_t *y_axis, int *year, int *month, int *day);

// netcdf_get_data : extrait la variable donnée et les axes associés
void netcdf_get_data(const char *filename, const char *var, float *data, float *x_labels, float *y_labels, char dimsname[2][NC_MAX_NAME + 1]);


#endif /* _NETCDF_API_H */