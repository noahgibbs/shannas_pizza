#include "xyz.h"
#include "xyz_variable.h"

/* An xyz_anim packages up variable evaluation, drawing and timekeeping
   into a single object meant to cause on-screen changes over time.
*/

typedef struct _xyz_anim_t xyz_anim;

typedef int (*xyz_anim_func)(xyz_anim *anim);

typedef struct {
  long duration_seconds;
  long duration_milliseconds;

  int repeat;

  xyz_anim_func construct;
  xyz_anim_func destroy;
  xyz_anim_func start;
  xyz_anim_func stop;
  xyz_anim_func evaluate;
  xyz_anim_func draw;
} xyz_anim_spec;

xyz_anim* xyz_anim_create(xyz_anim_spec *spec, void *user_info);
void xyz_anim_start(xyz_anim *anim);
void xyz_anim_evaluate(xyz_anim *anim);
void xyz_anim_draw(xyz_anim *anim);
void xyz_anim_stop(xyz_anim *anim);
void xyz_anim_delete(xyz_anim *anim);
void xyz_anim_delete_all(void);

void* xyz_anim_get_user_info(xyz_anim *anim);
double xyz_anim_get_current_ratio(xyz_anim *anim);

void xyz_anim_add_variable(xyz_anim *anim, xyz_variable *variable);

/* "Tick" one frame */
void xyz_anim_frame(xyz_anim *anim, long seconds, long milliseconds);
