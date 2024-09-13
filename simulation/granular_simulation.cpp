#include "granular_simulation.h"
#include <godot_cpp/variant/utility_functions.hpp>

#include "particles/basic/nothing.h"
#include "particles/basic/sand.h"

using namespace godot;

GranularSimulation::GranularSimulation() {
    UtilityFunctions::print("==Prepare simulation====================================");
    UtilityFunctions::print("==Initialize world grid=================================");
    initialize_grid(&particles);
}

GranularSimulation::~GranularSimulation() {}

void GranularSimulation::initialize_grid(std::vector<Particle*> *ps) {
    ps->resize(width*height);
    for (int i = 0; i < width*height; i++) {
        ps->at(i) = new Nothing();
    }

    render_data = PackedByteArray();
    render_data.resize(width * height * 3);
}

void GranularSimulation::step(int iterations) {
    for (int i = 0; i < iterations; i++) {
        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                particles[row * width + col]->set_is_updated(false);
            }
        }
        for (int row = height-1; row >=0; row--) {
            for (int col = 0; col < width; col++) {
                if (!particles[row * width + col]->get_is_updated())
                    particles[row * width + col]->update(this, row, col);
            }
        }
    }
}


void GranularSimulation::draw_particle(int row, int col, int typeID) {
    //UtilityFunctions::print("==Draw================================================");
    if (!is_in_bounds(row, col)) {
        return;
    }
    delete particles[row * width + col];
    particles[row * width + col] = new Sand();
}

void GranularSimulation::swap(int rowA, int colA, int rowB, int colB) {
    if (!is_in_bounds(rowA, colA) || !is_in_bounds(rowB, colB))
        return;
    
    Particle *tempP = particles[rowA * width + colA];
    particles[rowA * width + colA] = particles[rowB * width + colB];
    particles[rowB * width + colB] = tempP;
    tempP = NULL;

    particles[rowA * width + colA]->set_is_updated(true);
    particles[rowB * width + colB]->set_is_updated(true);
    
}

bool GranularSimulation::is_in_bounds(int row, int col) {
    return row >= 0 && col >= 0 && row < width && col < width;
}

bool GranularSimulation::is_swappable(int rowA, int colA, int rowB, int colB) {
    if (!is_in_bounds(rowA, colA) || !is_in_bounds(rowB, colB))
        return false;

    if (particles[rowA * width + colA]->get_density() <= particles[rowB * width + colB]->get_density())
        return false;

    return true;
}

inline float GranularSimulation::randf() {
    g_seed = (214013 * g_seed + 2531011); 
    return ((g_seed>>16) & 0x7FFF) / (double) 0x7FFF; 
}

Vector2i GranularSimulation::get_dimensions() {
    Vector2i dimensions = Vector2i(width, height);
    return dimensions;
}

PackedByteArray GranularSimulation::get_render_data() {
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            uint32_t col = particles[x * width + y]->get_color();
            
            int idx = (x * width + y) * 3;
            //convert hex to rgb
            render_data.set(idx, (col & 0xFF0000) >> 16);
            render_data.set(idx + 1, (col & 0x00FF00) >> 8);
            render_data.set(idx + 2, col & 0x0000FF);
        }
    }
    return render_data;
}

void GranularSimulation::_bind_methods() {
    ClassDB::bind_method(D_METHOD("step"), &GranularSimulation::step);
    ClassDB::bind_method(D_METHOD("draw_particle"), &GranularSimulation::draw_particle);
    ClassDB::bind_method(D_METHOD("get_dimensions"), &GranularSimulation::get_dimensions);
    ClassDB::bind_method(D_METHOD("get_render_data"), &GranularSimulation::get_render_data);
    ClassDB::bind_method(D_METHOD("is_in_bounds"), &GranularSimulation::is_in_bounds);
}
