#include <icetray/load_project.h>
namespace bp = boost::python;

void register_g4prop() {

}

I3_PYTHON_MODULE(g4prop) {
	load_project("g4prop",false);

	register_g4prop();
}
