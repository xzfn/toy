import vmath
import toy

import drawutil
from unit_manager import Unit


def format_part_slash_total(part_total):
	return '{}/{}'.format(part_total.part, part_total.total)


class EngineInfo(Unit):
	def tick(self, delta_time):
		debug_info = toy.app.render_manager.get_debug_info()
		render_culled_str = 'render culled:\n    ' + format_part_slash_total(debug_info.render_culled)
		depth_culled_strs = ['depth culled list:']
		for depth_culled in debug_info.depth_culled_list:
			depth_culled_strs.append('    ' + format_part_slash_total(depth_culled))
		drawutil.draw_screen_text(vmath.Vector3(0.0, 40.0, 0.0), render_culled_str)
		drawutil.draw_screen_text(vmath.Vector3(0.0, 80.0, 0.0), '\n'.join(depth_culled_strs))
