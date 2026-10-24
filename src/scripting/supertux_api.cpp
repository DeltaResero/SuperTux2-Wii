//  SuperTux
//  Copyright (C) 2026 DeltaResero
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "scripting/supertux_api.hpp"

#include <string>

#include <simplesquirrel/class.hpp>
#include <simplesquirrel/object.hpp>
#include <simplesquirrel/table.hpp>

#include "math/anchor_point.hpp"
#include "scripting/ambient_sound.hpp"
#include "scripting/background.hpp"
#include "scripting/badguy.hpp"
#include "scripting/camera.hpp"
#include "scripting/candle.hpp"
#include "scripting/clouds.hpp"
#include "scripting/custom_particles.hpp"
#include "scripting/decal.hpp"
#include "scripting/dispenser.hpp"
#include "scripting/display_effect.hpp"
#include "scripting/floating_image.hpp"
#include "scripting/functions.hpp"
#include "scripting/game_object_manager.hpp"
#include "scripting/gradient.hpp"
#include "scripting/level.hpp"
#include "scripting/level_time.hpp"
#include "scripting/particlesystem.hpp"
#include "scripting/platform.hpp"
#include "scripting/player.hpp"
#include "scripting/rain.hpp"
#include "scripting/rock.hpp"
#include "scripting/scripted_object.hpp"
#include "scripting/sector.hpp"
#include "scripting/spotlight.hpp"
#include "scripting/text.hpp"
#include "scripting/text_array.hpp"
#include "scripting/thunderstorm.hpp"
#include "scripting/tilemap.hpp"
#include "scripting/torch.hpp"
#include "scripting/willowisp.hpp"
#include "scripting/wind.hpp"
#include "scripting/worldmap.hpp"

namespace scripting {

namespace {

// simplesquirrel binds into a table it holds, so it gets a handle on the root table
ssq::Table root_table(HSQUIRRELVM v)
{
  ssq::Object root(v);
  sq_pushroottable(v);
  sq_getstackobj(v, -1, &root.getRaw());
  sq_addref(v, &root.getRaw());
  sq_pop(v, 1);
  return ssq::Table(root);
}

// For functions that read their arguments off the stack themselves
void add_raw_func(HSQUIRRELVM v, const char* name, SQFUNCTION func, const char* typemask)
{
  sq_pushstring(v, name, -1);
  sq_newclosure(v, func, 0);
  sq_setparamscheck(v, SQ_MATCHTYPEMASKSTRING, SQ_MATCHTYPEMASKSTRING, typemask);
  if (SQ_FAILED(sq_createslot(v, -3))) {
    throw SquirrelError(v, std::string("Couldn't register function '") + name + "'");
  }
}

} // namespace

SQInteger
release_hook(SQUserPointer ptr, SQInteger)
{
  delete static_cast<ssq::ExposableClass*>(ptr);
  return 0;
}

void
register_supertux_api(HSQUIRRELVM v)
{
  ssq::Table root = root_table(v);

  /* Constants */
  root.set<int>("ANCHOR_TOP", ANCHOR_TOP);
  root.set<int>("ANCHOR_BOTTOM", ANCHOR_BOTTOM);
  root.set<int>("ANCHOR_LEFT", ANCHOR_LEFT);
  root.set<int>("ANCHOR_RIGHT", ANCHOR_RIGHT);
  root.set<int>("ANCHOR_MIDDLE", ANCHOR_MIDDLE);
  root.set<int>("ANCHOR_TOP_LEFT", ANCHOR_TOP_LEFT);
  root.set<int>("ANCHOR_TOP_RIGHT", ANCHOR_TOP_RIGHT);
  root.set<int>("ANCHOR_BOTTOM_LEFT", ANCHOR_BOTTOM_LEFT);
  root.set<int>("ANCHOR_BOTTOM_RIGHT", ANCHOR_BOTTOM_RIGHT);

  /* Global functions */
  add_raw_func(v, "display", &display, "t.");
  root.addFunc("print_stacktrace", &print_stacktrace);
  add_raw_func(v, "get_current_thread", &get_current_thread, "t");
  root.addFunc("is_christmas", &is_christmas);
  root.addFunc("display_text_file", &display_text_file);
  root.addFunc("load_worldmap", &load_worldmap);
  root.addFunc("set_next_worldmap", &set_next_worldmap);
  root.addFunc("load_level", &load_level);
  root.addFunc("start_cutscene", &start_cutscene);
  root.addFunc("end_cutscene", &end_cutscene);
  root.addFunc("check_cutscene", &check_cutscene);
  root.addFunc("wait", &wait);
  root.addFunc("wait_for_screenswitch", &wait_for_screenswitch);
  root.addFunc("exit_screen", &exit_screen);
  root.addFunc("import", &import);
  root.addFunc("save_state", &save_state);
  root.addFunc("load_state", &load_state);
  root.addFunc("debug_collrects", &debug_collrects);
  root.addFunc("debug_show_fps", &debug_show_fps);
  root.addFunc("debug_draw_solids_only", &debug_draw_solids_only);
  root.addFunc("debug_worldmap_ghost", &debug_worldmap_ghost);
  root.addFunc("play_music", &play_music);
  root.addFunc("stop_music", &stop_music);
  root.addFunc("fade_in_music", &fade_in_music);
  root.addFunc("resume_music", &resume_music);
  root.addFunc("play_sound", &play_sound);
  root.addFunc("set_game_speed", &set_game_speed);
  root.addFunc("grease", &grease);
  root.addFunc("invincible", &invincible);
  root.addFunc("ghost", &ghost);
  root.addFunc("mortal", &mortal);
  root.addFunc("restart", &restart);
  root.addFunc("whereami", &whereami);
  root.addFunc("gotoend", &gotoend);
  root.addFunc("warp", &warp);
  root.addFunc("camera", &camera);
  root.addFunc("set_gamma", &set_gamma);
  root.addFunc("quit", &quit);
  root.addFunc("rand", &rand);
  root.addFunc("record_demo", &record_demo);
  root.addFunc("play_demo", &play_demo);
  root.addFunc("Level_finish", &Level_finish);
  root.addFunc("Level_spawn", &Level_spawn);
  root.addFunc("Level_flip_vertically", &Level_flip_vertically);
  root.addFunc("Level_toggle_pause", &Level_toggle_pause);

  /* Classes */

  ssq::Class ambient_sound = root.addAbstractClass<AmbientSound>("AmbientSound");
  ambient_sound.addFunc("set_pos", &AmbientSound::set_pos);
  ambient_sound.addFunc("get_pos_x", &AmbientSound::get_pos_x);
  ambient_sound.addFunc("get_pos_y", &AmbientSound::get_pos_y);

  ssq::Class background = root.addAbstractClass<Background>("Background");
  background.addFunc("set_image", &Background::set_image);
  background.addFunc("set_images", &Background::set_images);
  background.addFunc("set_speed", &Background::set_speed);
  background.addFunc("get_color_red", &Background::get_color_red);
  background.addFunc("get_color_green", &Background::get_color_green);
  background.addFunc("get_color_blue", &Background::get_color_blue);
  background.addFunc("get_color_alpha", &Background::get_color_alpha);
  background.addFunc("set_color", &Background::set_color);
  background.addFunc("fade_color", &Background::fade_color);

  ssq::Class bad_guy = root.addAbstractClass<BadGuy>("BadGuy");
  bad_guy.addFunc("kill", &BadGuy::kill);
  bad_guy.addFunc("ignite", &BadGuy::ignite);
  bad_guy.addFunc("set_action", &BadGuy::set_action);
  bad_guy.addFunc("set_sprite", &BadGuy::set_sprite);

  ssq::Class dispenser = root.addAbstractClass<Dispenser>("Dispenser", bad_guy);
  dispenser.addFunc("activate", &Dispenser::activate);
  dispenser.addFunc("deactivate", &Dispenser::deactivate);

  ssq::Class will_o_wisp = root.addAbstractClass<WillOWisp>("WillOWisp", bad_guy);
  will_o_wisp.addFunc("goto_node", &WillOWisp::goto_node);
  will_o_wisp.addFunc("set_state", &WillOWisp::set_state);
  will_o_wisp.addFunc("start_moving", &WillOWisp::start_moving);
  will_o_wisp.addFunc("stop_moving", &WillOWisp::stop_moving);

  ssq::Class camera = root.addAbstractClass<Camera>("Camera");
  camera.addFunc("reload_config", &Camera::reload_config);
  camera.addFunc("shake", &Camera::shake);
  camera.addFunc("set_pos", &Camera::set_pos);
  camera.addFunc("set_mode", &Camera::set_mode);
  camera.addFunc("scroll_to", &Camera::scroll_to);
  camera.addFunc("get_current_scale", &Camera::get_current_scale);
  camera.addFunc("get_target_scale", &Camera::get_target_scale);
  camera.addFunc("set_scale", &Camera::set_scale);
  camera.addFunc("scale", &Camera::scale);
  camera.addFunc("ease_scale", &Camera::ease_scale);

  ssq::Class candle = root.addAbstractClass<Candle>("Candle");
  candle.addFunc("get_burning", &Candle::get_burning);
  candle.addFunc("set_burning", &Candle::set_burning);

  ssq::Class clouds = root.addAbstractClass<Clouds>("Clouds");
  clouds.addFunc("set_enabled", &Clouds::set_enabled);
  clouds.addFunc("get_enabled", &Clouds::get_enabled);
  clouds.addFunc("fade_speed", &Clouds::fade_speed);
  clouds.addFunc("fade_amount", &Clouds::fade_amount);
  clouds.addFunc("set_amount", &Clouds::set_amount);

  ssq::Class custom_particles = root.addAbstractClass<CustomParticles>("CustomParticles");
  custom_particles.addFunc("set_enabled", &CustomParticles::set_enabled);
  custom_particles.addFunc("get_enabled", &CustomParticles::get_enabled);
  custom_particles.addFunc("clear", &CustomParticles::clear);
  custom_particles.addFunc("spawn_particles", &CustomParticles::spawn_particles);
  custom_particles.addFunc("get_max_amount", &CustomParticles::get_max_amount);
  custom_particles.addFunc("set_max_amount", &CustomParticles::set_max_amount);
  custom_particles.addFunc("get_birth_mode", &CustomParticles::get_birth_mode);
  custom_particles.addFunc("set_birth_mode", &CustomParticles::set_birth_mode);
  custom_particles.addFunc("get_death_mode", &CustomParticles::get_death_mode);
  custom_particles.addFunc("set_death_mode", &CustomParticles::set_death_mode);
  custom_particles.addFunc("get_rotation_mode", &CustomParticles::get_rotation_mode);
  custom_particles.addFunc("set_rotation_mode", &CustomParticles::set_rotation_mode);
  custom_particles.addFunc("get_collision_mode", &CustomParticles::get_collision_mode);
  custom_particles.addFunc("set_collision_mode", &CustomParticles::set_collision_mode);
  custom_particles.addFunc("get_offscreen_mode", &CustomParticles::get_offscreen_mode);
  custom_particles.addFunc("set_offscreen_mode", &CustomParticles::set_offscreen_mode);
  custom_particles.addFunc("get_cover_screen", &CustomParticles::get_cover_screen);
  custom_particles.addFunc("set_cover_screen", &CustomParticles::set_cover_screen);
  custom_particles.addFunc("get_delay", &CustomParticles::get_delay);
  custom_particles.addFunc("set_delay", &CustomParticles::set_delay);
  custom_particles.addFunc("fade_delay", &CustomParticles::fade_delay);
  custom_particles.addFunc("ease_delay", &CustomParticles::ease_delay);
  custom_particles.addFunc("get_lifetime", &CustomParticles::get_lifetime);
  custom_particles.addFunc("set_lifetime", &CustomParticles::set_lifetime);
  custom_particles.addFunc("fade_lifetime", &CustomParticles::fade_lifetime);
  custom_particles.addFunc("ease_lifetime", &CustomParticles::ease_lifetime);
  custom_particles.addFunc("get_lifetime_variation", &CustomParticles::get_lifetime_variation);
  custom_particles.addFunc("set_lifetime_variation", &CustomParticles::set_lifetime_variation);
  custom_particles.addFunc("fade_lifetime_variation", &CustomParticles::fade_lifetime_variation);
  custom_particles.addFunc("ease_lifetime_variation", &CustomParticles::ease_lifetime_variation);
  custom_particles.addFunc("get_birth_time", &CustomParticles::get_birth_time);
  custom_particles.addFunc("set_birth_time", &CustomParticles::set_birth_time);
  custom_particles.addFunc("fade_birth_time", &CustomParticles::fade_birth_time);
  custom_particles.addFunc("ease_birth_time", &CustomParticles::ease_birth_time);
  custom_particles.addFunc("get_birth_time_variation", &CustomParticles::get_birth_time_variation);
  custom_particles.addFunc("set_birth_time_variation", &CustomParticles::set_birth_time_variation);
  custom_particles.addFunc("fade_birth_time_variation", &CustomParticles::fade_birth_time_variation);
  custom_particles.addFunc("ease_birth_time_variation", &CustomParticles::ease_birth_time_variation);
  custom_particles.addFunc("get_death_time", &CustomParticles::get_death_time);
  custom_particles.addFunc("set_death_time", &CustomParticles::set_death_time);
  custom_particles.addFunc("fade_death_time", &CustomParticles::fade_death_time);
  custom_particles.addFunc("ease_death_time", &CustomParticles::ease_death_time);
  custom_particles.addFunc("get_death_time_variation", &CustomParticles::get_death_time_variation);
  custom_particles.addFunc("set_death_time_variation", &CustomParticles::set_death_time_variation);
  custom_particles.addFunc("fade_death_time_variation", &CustomParticles::fade_death_time_variation);
  custom_particles.addFunc("ease_death_time_variation", &CustomParticles::ease_death_time_variation);
  custom_particles.addFunc("get_speed_x", &CustomParticles::get_speed_x);
  custom_particles.addFunc("set_speed_x", &CustomParticles::set_speed_x);
  custom_particles.addFunc("fade_speed_x", &CustomParticles::fade_speed_x);
  custom_particles.addFunc("ease_speed_x", &CustomParticles::ease_speed_x);
  custom_particles.addFunc("get_speed_y", &CustomParticles::get_speed_y);
  custom_particles.addFunc("set_speed_y", &CustomParticles::set_speed_y);
  custom_particles.addFunc("fade_speed_y", &CustomParticles::fade_speed_y);
  custom_particles.addFunc("ease_speed_y", &CustomParticles::ease_speed_y);
  custom_particles.addFunc("get_speed_variation_x", &CustomParticles::get_speed_variation_x);
  custom_particles.addFunc("set_speed_variation_x", &CustomParticles::set_speed_variation_x);
  custom_particles.addFunc("fade_speed_variation_x", &CustomParticles::fade_speed_variation_x);
  custom_particles.addFunc("ease_speed_variation_x", &CustomParticles::ease_speed_variation_x);
  custom_particles.addFunc("get_speed_variation_y", &CustomParticles::get_speed_variation_y);
  custom_particles.addFunc("set_speed_variation_y", &CustomParticles::set_speed_variation_y);
  custom_particles.addFunc("fade_speed_variation_y", &CustomParticles::fade_speed_variation_y);
  custom_particles.addFunc("ease_speed_variation_y", &CustomParticles::ease_speed_variation_y);
  custom_particles.addFunc("get_acceleration_x", &CustomParticles::get_acceleration_x);
  custom_particles.addFunc("set_acceleration_x", &CustomParticles::set_acceleration_x);
  custom_particles.addFunc("fade_acceleration_x", &CustomParticles::fade_acceleration_x);
  custom_particles.addFunc("ease_acceleration_x", &CustomParticles::ease_acceleration_x);
  custom_particles.addFunc("get_acceleration_y", &CustomParticles::get_acceleration_y);
  custom_particles.addFunc("set_acceleration_y", &CustomParticles::set_acceleration_y);
  custom_particles.addFunc("fade_acceleration_y", &CustomParticles::fade_acceleration_y);
  custom_particles.addFunc("ease_acceleration_y", &CustomParticles::ease_acceleration_y);
  custom_particles.addFunc("get_friction_x", &CustomParticles::get_friction_x);
  custom_particles.addFunc("set_friction_x", &CustomParticles::set_friction_x);
  custom_particles.addFunc("fade_friction_x", &CustomParticles::fade_friction_x);
  custom_particles.addFunc("ease_friction_x", &CustomParticles::ease_friction_x);
  custom_particles.addFunc("get_friction_y", &CustomParticles::get_friction_y);
  custom_particles.addFunc("set_friction_y", &CustomParticles::set_friction_y);
  custom_particles.addFunc("fade_friction_y", &CustomParticles::fade_friction_y);
  custom_particles.addFunc("ease_friction_y", &CustomParticles::ease_friction_y);
  custom_particles.addFunc("get_feather_factor", &CustomParticles::get_feather_factor);
  custom_particles.addFunc("set_feather_factor", &CustomParticles::set_feather_factor);
  custom_particles.addFunc("fade_feather_factor", &CustomParticles::fade_feather_factor);
  custom_particles.addFunc("ease_feather_factor", &CustomParticles::ease_feather_factor);
  custom_particles.addFunc("get_rotation", &CustomParticles::get_rotation);
  custom_particles.addFunc("set_rotation", &CustomParticles::set_rotation);
  custom_particles.addFunc("fade_rotation", &CustomParticles::fade_rotation);
  custom_particles.addFunc("ease_rotation", &CustomParticles::ease_rotation);
  custom_particles.addFunc("get_rotation_variation", &CustomParticles::get_rotation_variation);
  custom_particles.addFunc("set_rotation_variation", &CustomParticles::set_rotation_variation);
  custom_particles.addFunc("fade_rotation_variation", &CustomParticles::fade_rotation_variation);
  custom_particles.addFunc("ease_rotation_variation", &CustomParticles::ease_rotation_variation);
  custom_particles.addFunc("get_rotation_speed", &CustomParticles::get_rotation_speed);
  custom_particles.addFunc("set_rotation_speed", &CustomParticles::set_rotation_speed);
  custom_particles.addFunc("fade_rotation_speed", &CustomParticles::fade_rotation_speed);
  custom_particles.addFunc("ease_rotation_speed", &CustomParticles::ease_rotation_speed);
  custom_particles.addFunc("get_rotation_speed_variation", &CustomParticles::get_rotation_speed_variation);
  custom_particles.addFunc("set_rotation_speed_variation", &CustomParticles::set_rotation_speed_variation);
  custom_particles.addFunc("fade_rotation_speed_variation", &CustomParticles::fade_rotation_speed_variation);
  custom_particles.addFunc("ease_rotation_speed_variation", &CustomParticles::ease_rotation_speed_variation);
  custom_particles.addFunc("get_rotation_acceleration", &CustomParticles::get_rotation_acceleration);
  custom_particles.addFunc("set_rotation_acceleration", &CustomParticles::set_rotation_acceleration);
  custom_particles.addFunc("fade_rotation_acceleration", &CustomParticles::fade_rotation_acceleration);
  custom_particles.addFunc("ease_rotation_acceleration", &CustomParticles::ease_rotation_acceleration);
  custom_particles.addFunc("get_rotation_decceleration", &CustomParticles::get_rotation_decceleration);
  custom_particles.addFunc("set_rotation_decceleration", &CustomParticles::set_rotation_decceleration);
  custom_particles.addFunc("fade_rotation_decceleration", &CustomParticles::fade_rotation_decceleration);
  custom_particles.addFunc("ease_rotation_decceleration", &CustomParticles::ease_rotation_decceleration);

  ssq::Class decal = root.addAbstractClass<Decal>("Decal");
  decal.addFunc("fade_sprite", &Decal::fade_sprite);
  decal.addFunc("change_sprite", &Decal::change_sprite);
  decal.addFunc("fade_in", &Decal::fade_in);
  decal.addFunc("fade_out", &Decal::fade_out);

  ssq::Class display_effect = root.addAbstractClass<DisplayEffect>("DisplayEffect");
  display_effect.addFunc("fade_out", &DisplayEffect::fade_out);
  display_effect.addFunc("fade_in", &DisplayEffect::fade_in);
  display_effect.addFunc("set_black", &DisplayEffect::set_black);
  display_effect.addFunc("is_black", &DisplayEffect::is_black);
  display_effect.addFunc("sixteen_to_nine", &DisplayEffect::sixteen_to_nine);
  display_effect.addFunc("four_to_three", &DisplayEffect::four_to_three);

  ssq::Class floating_image = root.addClass<FloatingImage>("FloatingImage",
      ssq::Class::Ctor<FloatingImage(const std::string&)>());
  floating_image.addFunc("set_layer", &FloatingImage::set_layer);
  floating_image.addFunc("get_layer", &FloatingImage::get_layer);
  floating_image.addFunc("set_pos", &FloatingImage::set_pos);
  floating_image.addFunc("get_pos_x", &FloatingImage::get_pos_x);
  floating_image.addFunc("get_pos_y", &FloatingImage::get_pos_y);
  floating_image.addFunc("set_anchor_point", &FloatingImage::set_anchor_point);
  floating_image.addFunc("get_anchor_point", &FloatingImage::get_anchor_point);
  floating_image.addFunc("set_visible", &FloatingImage::set_visible);
  floating_image.addFunc("get_visible", &FloatingImage::get_visible);
  floating_image.addFunc("set_action", &FloatingImage::set_action);
  floating_image.addFunc("get_action", &FloatingImage::get_action);
  floating_image.addFunc("fade_in", &FloatingImage::fade_in);
  floating_image.addFunc("fade_out", &FloatingImage::fade_out);

  ssq::Class game_object_manager = root.addAbstractClass<GameObjectManager>("GameObjectManager");
  game_object_manager.addFunc("set_ambient_light", &GameObjectManager::set_ambient_light);
  game_object_manager.addFunc("fade_to_ambient_light", &GameObjectManager::fade_to_ambient_light);
  game_object_manager.addFunc("get_ambient_red", &GameObjectManager::get_ambient_red);
  game_object_manager.addFunc("get_ambient_green", &GameObjectManager::get_ambient_green);
  game_object_manager.addFunc("get_ambient_blue", &GameObjectManager::get_ambient_blue);
  game_object_manager.addFunc("set_music", &GameObjectManager::set_music);

  ssq::Class sector = root.addAbstractClass<Sector>("Sector", game_object_manager);
  sector.addFunc("set_gravity", &Sector::set_gravity);

  root.addAbstractClass<WorldMap>("WorldMap", game_object_manager);

  ssq::Class gradient = root.addAbstractClass<Gradient>("Gradient");
  gradient.addFunc("set_direction", &Gradient::set_direction);
  gradient.addFunc("get_direction", &Gradient::get_direction);
  gradient.addFunc("set_color1", &Gradient::set_color1);
  gradient.addFunc("set_color2", &Gradient::set_color2);
  gradient.addFunc("set_colors", &Gradient::set_colors);
  gradient.addFunc("fade_color1", &Gradient::fade_color1);
  gradient.addFunc("fade_color2", &Gradient::fade_color2);
  gradient.addFunc("fade_colors", &Gradient::fade_colors);
  gradient.addFunc("swap_colors", &Gradient::swap_colors);

  ssq::Class level_time = root.addAbstractClass<LevelTime>("LevelTime");
  level_time.addFunc("start", &LevelTime::start);
  level_time.addFunc("stop", &LevelTime::stop);
  level_time.addFunc("get_time", &LevelTime::get_time);
  level_time.addFunc("set_time", &LevelTime::set_time);

  ssq::Class particle_system = root.addAbstractClass<ParticleSystem>("ParticleSystem");
  particle_system.addFunc("set_enabled", &ParticleSystem::set_enabled);
  particle_system.addFunc("get_enabled", &ParticleSystem::get_enabled);

  ssq::Class platform = root.addAbstractClass<Platform>("Platform");
  platform.addFunc("goto_node", &Platform::goto_node);
  platform.addFunc("start_moving", &Platform::start_moving);
  platform.addFunc("stop_moving", &Platform::stop_moving);
  platform.addFunc("set_action", &Platform::set_action);

  ssq::Class player = root.addAbstractClass<Player>("Player");
  player.addFunc("add_bonus", &Player::add_bonus);
  player.addFunc("set_bonus", &Player::set_bonus);
  player.addFunc("add_coins", &Player::add_coins);
  player.addFunc("get_coins", &Player::get_coins);
  player.addFunc("make_invincible", &Player::make_invincible);
  player.addFunc("deactivate", &Player::deactivate);
  player.addFunc("activate", &Player::activate);
  player.addFunc("walk", &Player::walk);
  player.addFunc("set_dir", &Player::set_dir);
  player.addFunc("set_visible", &Player::set_visible);
  player.addFunc("get_visible", &Player::get_visible);
  player.addFunc("kill", &Player::kill);
  player.addFunc("set_ghost_mode", &Player::set_ghost_mode);
  player.addFunc("get_ghost_mode", &Player::get_ghost_mode);
  player.addFunc("kick", &Player::kick);
  player.addFunc("do_cheer", &Player::do_cheer);
  player.addFunc("do_duck", &Player::do_duck);
  player.addFunc("do_standup", &Player::do_standup);
  player.addFunc("do_backflip", &Player::do_backflip);
  player.addFunc("do_jump", &Player::do_jump);
  player.addFunc("trigger_sequence", &Player::trigger_sequence);
  player.addFunc("use_scripting_controller", &Player::use_scripting_controller);
  player.addFunc("has_grabbed", &Player::has_grabbed);
  player.addFunc("do_scripting_controller", &Player::do_scripting_controller);
  player.addFunc("get_velocity_x", &Player::get_velocity_x);
  player.addFunc("get_velocity_y", &Player::get_velocity_y);
  player.addFunc("get_x", &Player::get_x);
  player.addFunc("get_y", &Player::get_y);
  player.addFunc("set_pos", &Player::set_pos);

  ssq::Class rain = root.addAbstractClass<Rain>("Rain");
  rain.addFunc("set_enabled", &Rain::set_enabled);
  rain.addFunc("get_enabled", &Rain::get_enabled);
  rain.addFunc("fade_speed", &Rain::fade_speed);
  rain.addFunc("fade_amount", &Rain::fade_amount);
  rain.addFunc("fade_angle", &Rain::fade_angle);

  ssq::Class rock = root.addAbstractClass<Rock>("Rock");

  ssq::Class scripted_object = root.addAbstractClass<ScriptedObject>("ScriptedObject");
  scripted_object.addFunc("set_action", &ScriptedObject::set_action);
  scripted_object.addFunc("get_action", &ScriptedObject::get_action);
  scripted_object.addFunc("move", &ScriptedObject::move);
  scripted_object.addFunc("set_pos", &ScriptedObject::set_pos);
  scripted_object.addFunc("get_pos_x", &ScriptedObject::get_pos_x);
  scripted_object.addFunc("get_pos_y", &ScriptedObject::get_pos_y);
  scripted_object.addFunc("set_velocity", &ScriptedObject::set_velocity);
  scripted_object.addFunc("get_velocity_x", &ScriptedObject::get_velocity_x);
  scripted_object.addFunc("get_velocity_y", &ScriptedObject::get_velocity_y);
  scripted_object.addFunc("enable_gravity", &ScriptedObject::enable_gravity);
  scripted_object.addFunc("gravity_enabled", &ScriptedObject::gravity_enabled);
  scripted_object.addFunc("set_visible", &ScriptedObject::set_visible);
  scripted_object.addFunc("is_visible", &ScriptedObject::is_visible);
  scripted_object.addFunc("set_solid", &ScriptedObject::set_solid);
  scripted_object.addFunc("is_solid", &ScriptedObject::is_solid);
  scripted_object.addFunc("get_name", &ScriptedObject::get_name);

  ssq::Class spotlight = root.addAbstractClass<Spotlight>("Spotlight");
  spotlight.addFunc("set_direction", &Spotlight::set_direction);
  spotlight.addFunc("set_angle", &Spotlight::set_angle);
  spotlight.addFunc("fade_angle", &Spotlight::fade_angle);
  spotlight.addFunc("ease_angle", &Spotlight::ease_angle);
  spotlight.addFunc("set_speed", &Spotlight::set_speed);
  spotlight.addFunc("fade_speed", &Spotlight::fade_speed);
  spotlight.addFunc("ease_speed", &Spotlight::ease_speed);
  spotlight.addFunc("set_color_rgba", &Spotlight::set_color_rgba);
  spotlight.addFunc("fade_color_rgba", &Spotlight::fade_color_rgba);
  spotlight.addFunc("ease_color_rgba", &Spotlight::ease_color_rgba);

  ssq::Class text = root.addAbstractClass<Text>("Text");
  text.addFunc("set_text", &Text::set_text);
  text.addFunc("set_font", &Text::set_font);
  text.addFunc("fade_in", &Text::fade_in);
  text.addFunc("fade_out", &Text::fade_out);
  text.addFunc("set_visible", &Text::set_visible);
  text.addFunc("set_centered", &Text::set_centered);
  text.addFunc("set_pos", &Text::set_pos);
  text.addFunc("get_pos_x", &Text::get_pos_x);
  text.addFunc("get_pos_y", &Text::get_pos_y);
  text.addFunc("set_anchor_point", &Text::set_anchor_point);
  text.addFunc("get_anchor_point", &Text::get_anchor_point);

  ssq::Class text_array = root.addAbstractClass<TextArray>("TextArray");
  text_array.addFunc("clear", &TextArray::clear);
  text_array.addFunc("add_text", &TextArray::add_text);
  text_array.addFunc("add_text_duration", &TextArray::add_text_duration);
  text_array.addFunc("set_text_index", &TextArray::set_text_index);
  text_array.addFunc("set_keep_visible", &TextArray::set_keep_visible);
  text_array.addFunc("set_fade_transition", &TextArray::set_fade_transition);
  text_array.addFunc("set_fade_time", &TextArray::set_fade_time);
  text_array.addFunc("set_done", &TextArray::set_done);
  text_array.addFunc("set_auto", &TextArray::set_auto);
  text_array.addFunc("next_text", &TextArray::next_text);
  text_array.addFunc("prev_text", &TextArray::prev_text);
  text_array.addFunc("set_text", &TextArray::set_text);
  text_array.addFunc("set_font", &TextArray::set_font);
  text_array.addFunc("fade_in", &TextArray::fade_in);
  text_array.addFunc("fade_out", &TextArray::fade_out);
  text_array.addFunc("set_visible", &TextArray::set_visible);
  text_array.addFunc("set_centered", &TextArray::set_centered);
  text_array.addFunc("set_pos", &TextArray::set_pos);
  text_array.addFunc("get_pos_x", &TextArray::get_pos_x);
  text_array.addFunc("get_pos_y", &TextArray::get_pos_y);
  text_array.addFunc("set_anchor_point", &TextArray::set_anchor_point);
  text_array.addFunc("get_anchor_point", &TextArray::get_anchor_point);

  ssq::Class thunderstorm = root.addAbstractClass<Thunderstorm>("Thunderstorm");
  thunderstorm.addFunc("start", &Thunderstorm::start);
  thunderstorm.addFunc("stop", &Thunderstorm::stop);
  thunderstorm.addFunc("thunder", &Thunderstorm::thunder);
  thunderstorm.addFunc("lightning", &Thunderstorm::lightning);
  thunderstorm.addFunc("flash", &Thunderstorm::flash);
  thunderstorm.addFunc("electrify", &Thunderstorm::electrify);

  ssq::Class tile_map = root.addAbstractClass<TileMap>("TileMap");
  tile_map.addFunc("goto_node", &TileMap::goto_node);
  tile_map.addFunc("start_moving", &TileMap::start_moving);
  tile_map.addFunc("stop_moving", &TileMap::stop_moving);
  tile_map.addFunc("get_tile_id", &TileMap::get_tile_id);
  tile_map.addFunc("get_tile_id_at", &TileMap::get_tile_id_at);
  tile_map.addFunc("change", &TileMap::change);
  tile_map.addFunc("change_at", &TileMap::change_at);
  tile_map.addFunc("fade", &TileMap::fade);
  tile_map.addFunc("tint_fade", &TileMap::tint_fade);
  tile_map.addFunc("set_alpha", &TileMap::set_alpha);
  tile_map.addFunc("get_alpha", &TileMap::get_alpha);
  tile_map.addFunc("set_solid", &TileMap::set_solid);

  ssq::Class torch = root.addAbstractClass<Torch>("Torch");
  torch.addFunc("get_burning", &Torch::get_burning);
  torch.addFunc("set_burning", &Torch::set_burning);

  ssq::Class wind = root.addAbstractClass<Wind>("Wind");
  wind.addFunc("start", &Wind::start);
  wind.addFunc("stop", &Wind::stop);
}

} // namespace scripting

/* EOF */
