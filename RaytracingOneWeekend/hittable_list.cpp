#include "hittable_list.h"

hittable_list::hittable_list(hittable** l, int n) { list = l; list_size = n; }

bool hittable_list::hit(const ray& r, float t_min, float t_max, hit_record& rec)const {
	hit_record temp_rec;
	bool hitted = false;
	double closest_so_far = t_max;

	for (int i = 0; i < list_size; i++) {
		if (list[i]->hit(r, t_min, closest_so_far, temp_rec)) {
			hitted = true;
			closest_so_far = temp_rec.t;
			rec = temp_rec;
		}
	}

	return hitted;
}