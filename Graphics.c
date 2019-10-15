#include "Graphics.h"

#include "debugmalloc.h"

#define set_pixel(dest, x, y, c) (((x)>=0&&(x)<(dest)->w&&(y)>=0&&(y)<(dest)->h)?((uint32_t *)(dest)->pixels)[(y) * (dest)->w + (x)] = (c):0)
#define int_to_color(color) ((SDL_Color){(color) >> 24u, ((color) >> 16u) & 0xffu, ((color) >> 8u) & 0xffu, (color) & 0xffu})

//Source: https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
void __gfx_draw_line_low(SDL_Surface *dest, float x0, float y0, float x1, float y1, Color color) {
	float dx = x1 - x0;
	float dy = y1 - y0;
	float yi = 1;
	if (dy < 0) {
		yi = -1;
		dy = -dy;
	}
	float err = 2 * dy - dx;
	float y = y0;

	int xi = 1;
	if (dx < 0)
		xi = -1;

	for (int x = (int)x0; x != (int)x1; x += xi) {
		set_pixel(dest, (int)x, (int)y, color);
		if (err > 0) {
			y += yi;
			err -= 2 * dx;
		}
		err += 2 * dy;
	}
}

void __gfx_draw_line_high(SDL_Surface *dest, float x0, float y0, float x1, float y1, Color color) {
	float dx = x1 - x0;
	float dy = y1 - y0;
	float xi = 1;
	if (dx < 0) {
		xi = -1;
		dx = -dx;
	}
	float err = 2 * dx - dy;
	float x = x0;

	int yi = 1;
	if (dy < 0)
		yi = -1;

	for (int y = (int)y0; y != (int)y1; y += yi) {
		set_pixel(dest, (int)x, (int)y, color);
		if (err > 0) {
			x += xi;
			err -= 2 * dy;
		}
		err += 2 * dx;
	}
}

void gfx_draw_line(SDL_Surface *dest, Point V1, Point V2, Color color) {
	if (V1.x == V2.x) {
		if (V1.y < V2.y)
			for (int y = (int)V1.y; (float)y <= V2.y; y++) {
				set_pixel(dest, (int) V1.x, y, color);
			}
		else
			for (int y = (int)V2.y; (float)y <= V1.y; y++) {
				set_pixel(dest, (int) V1.x, y, color);
			}
		return;
	}
	if (V1.y == V2.y) {
		if (V1.x < V2.x)
			for (int x = (int)V1.x; (float)x <= V2.x; x++) {
				set_pixel(dest, x, (int) V1.y, color);
			}
		else
			for (int x = (int)V2.x; (float)x <= V1.x; x++) {
				set_pixel(dest, x, (int) V1.y, color);
			}
		return;
	}

	if (fabsf(V2.y - V1.y) < fabsf(V2.x - V1.x)) {
		if (V1.x > V2.x)
			__gfx_draw_line_low(dest, V2.x, V2.y, V1.x, V1.y, color);
		else
			__gfx_draw_line_low(dest, V1.x, V1.y, V2.x, V2.y, color);
	} else {
		if (V1.y > V2.y)
			__gfx_draw_line_high(dest, V2.x, V2.y, V1.x, V1.y, color);
		else
			__gfx_draw_line_high(dest, V1.x, V1.y, V2.x, V2.y, color);
	}
}

void gfx_draw_triangle(SDL_Surface *dest, Point V1, Point V2, Point V3, Color color) {
	gfx_draw_line(dest, V1, V2, color);
	gfx_draw_line(dest, V1, V3, color);
	gfx_draw_line(dest, V2, V3, color);
}

//Source: http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html#algo2
void __fill_flatsided_triangle(SDL_Surface *dest, Point V1, Point V2, Point V3, Color color) {
	Point VT1 = V1;
	Point VT2 = V1;

	bool c1 = false, c2 = false;

	float dx1 = fabsf(V2.x - V1.x);
	float dy1 = fabsf(V2.y - V1.y);

	float dx2 = fabsf(V3.x - V1.x);
	float dy2 = fabsf(V3.y - V1.y);

	float signx1 = (float)((V2.x - V1.x > 0) - (V2.x - V1.x < 0));
	float signx2 = (float)((V3.x - V1.x > 0) - (V3.x - V1.x < 0));

	float signy1 = (float)((V2.y - V1.y > 0) - (V2.y - V1.y < 0));
	float signy2 = (float)((V3.y - V1.y > 0) - (V3.y - V1.y < 0));

	if (dy1 > dx1) {
		float temp = dx1;
		dx1 = dy1;
		dy1 = temp;
		c1 = true;
	}

	if (dy2 > dx2) {
		float temp = dx2;
		dx2 = dy2;
		dy2 = temp;
		c2 = true;
	}

	float e1 = 2 * dy1 - dx1;
	float e2 = 2 * dy2 - dx2;

	for (int i = 0; (float)i <= dx1; i++) {
		gfx_draw_line(dest, VT1, VT2, color);

		while (e1 >= 0) {
			if (c1)
				VT1.x += signx1;
			else
				VT1.y += signy1;
			e1 -= 2 * dx1;
		}

		if (c1)
			VT1.y += signy1;
		else
			VT1.x += signx1;

		e1 += 2 * dy1;

		while (VT2.y != VT1.y) {
			while (e2 >= 0) {
				if (c2)
					VT2.x += signx2;
				else
					VT2.y += signy2;
				e2 -= 2 * dx2;
			}

			if (c2)
				VT2.y += signy2;
			else
				VT2.x += signx2;
			e2 += 2 * dy2;
		}
	}
}

void gfx_fill_triangle(SDL_Surface *dest, Point V1, Point V2, Point V3, Color color) {

	if (V1.y > V2.y) {
		Point temp = V1;
		V1 = V2;
		V2 = temp;
	}
	if (V2.y > V3.y) {
		Point temp = V2;
		V2 = V3;
		V3 = temp;
	}
	if (V1.y > V2.y) {
		Point temp = V1;
		V1 = V2;
		V2 = temp;
	}

	if (V2.y == V3.y) {
		__fill_flatsided_triangle(dest, V1, V2, V3, color);
	}
	else if (V1.y == V2.y) {
		__fill_flatsided_triangle(dest, V3, V1, V2, color);
	}
	else {
		Point V4 = {
				V1.x + ((V2.y - V1.y) / (V3.y - V1.y)) * (V3.x - V1.x),
				V2.y
		};
		__fill_flatsided_triangle(dest, V1, V2, V4, color);
		__fill_flatsided_triangle(dest, V3, V2, V4, color);
	}
}

void gfx_draw_thick_line(SDL_Surface *dest, Point V1, Point V2, float thickness, Color color) {

	if (V1.y == V2.y) {
		for (int x = (int)V1.x; x <= (int)V2.x; x++) {
			for (int y = (int)(V1.y - thickness/2); y <= (int)(V1.y + thickness / 2); y++)
				set_pixel(dest, x, y, color);
		}
		return;
	}

	if (V1.x == V2.x) {
		for (int x = (int)(V1.x - thickness / 2); x <= (int)(V1.x + thickness / 2); x++) {
			for (int y = (int)V1.y; y <= (int)V2.y; y++)
				set_pixel(dest, x, y, color);
		}
		return;
	}

	float l = 1 / sqrtf((V2.x - V1.x) * (V2.x - V1.x) + (V2.y - V1.y) * (V2.y - V1.y));
	thickness = thickness / 2;
	float dx = (V2.x - V1.x) * l * thickness;
	float dy = (V2.y - V1.y) * l * thickness;

	gfx_fill_triangle(dest,
					  (Point){V1.x + dx, V1.y - dy},
					  (Point){V1.x - dx, V1.y + dy},
					  (Point){V2.x + dx, V2.y - dy},
					  color);
	gfx_fill_triangle(dest,
					  (Point){V2.x + dx, V2.y - dy},
					  (Point){V2.x - dx, V2.y + dy},
					  (Point){V1.x - dx, V1.y + dy},
					  color);
}

void __draw_circle_helper(SDL_Surface *dest, int cx, int cy, int x, int y, Color color) {
	set_pixel(dest, cx + x, cy + y, color);
	set_pixel(dest, cx - x, cy + y, color);
	set_pixel(dest, cx + x, cy - y, color);
	set_pixel(dest, cx - x, cy - y, color);
	set_pixel(dest, cx + y, cy + x, color);
	set_pixel(dest, cx - y, cy + x, color);
	set_pixel(dest, cx + y, cy - x, color);
	set_pixel(dest, cx - y, cy - x, color);
}

void gfx_draw_circle(SDL_Surface *dest, Point C, float r, Color color) {
	int x = 0, y = (int)r;
	int d = 3 - 2 * (int)r;
	__draw_circle_helper(dest, (int)C.x, (int)C.y, x, y, color);
	while (y >= x) {
		x++;
		if (d > 0) {
			y--;
			d += 4 * (x - y) + 10;
		} else
			d += 4 * x + 6;
		__draw_circle_helper(dest, (int)C.x, (int)C.y, x, y, color);
	}
}

void __fill_circle_helper(SDL_Surface *dest, int cx, int cy, int x, int y, Color color) {
	for (int dx = -x; dx <= x; dx++) {
		set_pixel(dest, cx + dx, cy + y, color);
		set_pixel(dest, cx + dx, cy - y, color);
	}
	for (int dy = -y; dy <= y; dy++) {
		set_pixel(dest, cx + dy, cy + x, color);
		set_pixel(dest, cx + dy, cy - x, color);
	}
}

void gfx_fill_circle(SDL_Surface *dest, Point C, float r, Color color) {
	int x = 0, y = (int)r;
	int d = 3 - 2 * (int)r;
	__fill_circle_helper(dest, (int)C.x, (int)C.y, x, y, color);
	while (y >= x) {
		x++;
		if (d > 0) {
			y--;
			d += 4 * (x - y) + 10;
		} else
			d += 4 * x + 6;
		__fill_circle_helper(dest, (int)C.x, (int)C.y, x, y, color);
	}
}

void __gfx_calc_bezier_point(float Ax, float Bx, float Cx, float Ay, float By, float Cy,
		float thickness, float t, Point *Q1, Point *Q2) {
	Point Q = {
			Ax * t * t + 2 * Bx * t + Cx,
			Ay * t * t + 2 * By * t + Cy
	};
	float dx = 2 * Ax * t + 2 * Bx;
	float dy = 2 * Ay * t + 2 * By;
	float l = 1 / sqrtf(dx * dx + dy * dy);
	dx *= l;
	dy *= l;
	*Q1 = (Point){
			floorf(Q.x + dy * thickness), floorf(Q.y - dx * thickness)
	};
	*Q2 = (Point){
			floorf(Q.x - dy * thickness), floorf(Q.y + dx * thickness)
	};
}

void gfx_draw_bezier(SDL_Surface *dest, Point V1, Point V2, Point C, float thickness, Color color) {
	static const float stepSize = 0.01f;
	float Ax = V1.x - 2 * C.x + V2.x;
	float Ay = V1.y - 2 * C.y + V2.y;
	float Bx = C.x - V1.x;
	float By = C.y - V1.y;
	float Cx = V1.x;
	float Cy = V1.y;

	Point prevQ1, prevQ2;
	__gfx_calc_bezier_point(Ax, Bx, Cx, Ay, By, Cy, thickness / 2, 0, &prevQ1, &prevQ2);

	//A Clion nyavalyog, ha for loopban float a conditional változó, így while lett
	float t = 0;
	while (t <= 1) {
		printf("%f\n", t);
		Point Q1, Q2;
		__gfx_calc_bezier_point(Ax, Bx, Cx, Ay, By, Cy, thickness / 2, t, &Q1, &Q2);

		gfx_fill_triangle(dest, Q1, Q2, prevQ1, color);
		gfx_fill_triangle(dest, Q2, prevQ1, prevQ2, color);
		t += stepSize;
		prevQ1 = Q1;
		prevQ2 = Q2;
	}
}

void __gfx_calc_bezier_point_cubic(Point P0, Point P1, Point P2, Point P3,
								float thickness, float t, Point *Q1, Point *Q2) {
	Point Q = {
			(1 - t) * (1 - t) * (1 - t) * P0.x + 3 * (1 - t) * (1 - t) * t * P1.x + 3 * (1 - t) * t * t * P2.x + t * t * t * P3.x,
			(1 - t) * (1 - t) * (1 - t) * P0.y + 3 * (1 - t) * (1 - t) * t * P1.y + 3 * (1 - t) * t * t * P2.y + t * t * t * P3.y
	};
	float dx = 3 * (1 - t) * (1 - t) * (P1.x - P0.x) + 6 * (1 - t) * (1 - t) * t * (P2.x - P1.x) + 3 * t * t * (P3.x - P2.x);
	float dy = 3 * (1 - t) * (1 - t) * (P1.y - P0.y) + 6 * (1 - t) * (1 - t) * t * (P2.y - P1.y) + 3 * t * t * (P3.y - P2.y);
	float l = 1 / sqrtf(dx * dx + dy * dy);
	dx *= l;
	dy *= l;
	*Q1 = (Point){
			floorf(Q.x + dy * thickness), floorf(Q.y - dx * thickness)
	};
	*Q2 = (Point){
			floorf(Q.x - dy * thickness), floorf(Q.y + dx * thickness)
	};
}

void gfx_draw_bezier_cubic(SDL_Surface *dest, Point V1, Point V2, Point C1, Point C2, float thickness, Color color) {
	static const float stepSize = 0.005f;

	Point prevQ1, prevQ2;
	__gfx_calc_bezier_point_cubic(V1, C1, C2, V2, thickness / 2, 0, &prevQ1, &prevQ2);

	//A Clion nyavalyog, ha for loopban float a conditional változó, így while lett
	float t = 0;
	while (t <= 1) {
		t += stepSize;
		Point Q1, Q2;
		__gfx_calc_bezier_point_cubic(V1, C1, C2, V2, thickness / 2, t, &Q1, &Q2);

		gfx_fill_triangle(dest, Q1, Q2, prevQ1, color);
		gfx_fill_triangle(dest, Q2, prevQ1, prevQ2, color);
		t += stepSize;
		prevQ1 = Q1;
		prevQ2 = Q2;
	}
}

void gfx_draw_text(SDL_Surface *dest, TTF_Font *font, int x, int y, const char *str, Color color) {
	SDL_Surface *text = TTF_RenderText_Blended(font, str, int_to_color(color));
	SDL_Rect r = {x, y, 0, 0};
	SDL_BlitSurface(text, NULL, dest, &r);
	SDL_FreeSurface(text);
}

void gfx_fill_ring(SDL_Surface *dest, Point C, float r, float thickness, Color color) {
	float ri = r - thickness / 2;
	float ro = r + thickness / 2;

for (int dy = 0; dy <= (int)floorf(ro); dy++) {
		if ((float)dy > ri) {
			float dx = sqrtf(ro * ro - (float)(dy * dy));
			gfx_draw_line(dest, (Point){C.x - dx, C.y + (float)dy}, (Point){C.x + dx, C.y + (float)dy}, color);
			gfx_draw_line(dest, (Point){C.x - dx, C.y - (float)dy}, (Point){C.x + dx, C.y - (float)dy}, color);
		} else {
			float dxo = sqrtf(ro * ro - (float)(dy * dy));
			float dxi = sqrtf(ri * ri - (float)(dy * dy));
			gfx_draw_line(dest, (Point){C.x - dxo, C.y + (float)dy}, (Point){C.x - dxi, C.y + (float)dy}, color);
			gfx_draw_line(dest, (Point){C.x + dxi, C.y + (float)dy}, (Point){C.x + dxo, C.y + (float)dy}, color);
			gfx_draw_line(dest, (Point){C.x - dxo, C.y - (float)dy}, (Point){C.x - dxi, C.y - (float)dy}, color);
			gfx_draw_line(dest, (Point){C.x + dxi, C.y - (float)dy}, (Point){C.x + dxo, C.y - (float)dy}, color);
		}
	}
}