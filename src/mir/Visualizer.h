#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

#include "mir/MIR.h"

namespace mir {

class Visualizer {
public:
	static std::string to_dot(const mir::Module &mod) {
		std::stringstream ss;
		ss << "digraph " << mod.name << " {\n";
		ss << "  node [shape=record, fontname=\"Courier New\"];\n";

		for (auto &func : mod.functions) {
			ss << "  subgraph cluster_" << func->name << " {\n";
			ss << "    label = \"Function: " << func->name << "\";\n";

			for (auto &bb : func->blocks) {
				ss << "    " << bb->label << " [label=\"{" << bb->label << "|";

				for (auto &inst : bb->instructions) {
					ss << escape_html(inst_to_string(inst)) << "\\l";
				}
				ss << "}\"];\n";

				// Draw Edges based on Terminators
				if (!bb->instructions.empty()) {
					auto &last = bb->instructions.back();
					if (last.kind == InstKind::Jump) {
						ss << "    " << bb->label << " -> " << last.args[0] << ";\n";
					} else if (last.kind == InstKind::Branch) {
						ss << "    " << bb->label << " -> " << last.args[1]
						   << " [label=\"true\"];\n";
						ss << "    " << bb->label << " -> " << last.args[2]
						   << " [label=\"false\"];\n";
					}
				}
			}
			ss << "  }\n";
		}
		ss << "}\n";
		return ss.str();
	}

private:
	static std::string inst_to_string(const Instruction &inst) {
		std::string s;
		if (inst.dest)
			s += std::format("{} = ", *inst.dest);

		switch (inst.kind) {
			case InstKind::Retain:	 s += "RETAIN "; break;
			case InstKind::Release:	 s += "RELEASE "; break;
			case InstKind::Assign:	 s += "copy "; break;
			case InstKind::LoadLit:	 s += "load_lit "; break;
			case InstKind::UnaryOp:	 s += "unary "; break;
			case InstKind::BinaryOp: s += "binary "; break;
			case InstKind::Call:	 s += "call "; break;
			case InstKind::Jump:	 s += "goto "; break;
			case InstKind::Branch:	 s += "if "; break;
			case InstKind::Return:	 s += "ret "; break;
			default:				 s += "unknown "; break;
		}

		for (auto &arg : inst.args)
			s += std::format("{} ", arg);

		std::replace(s.begin(), s.end(), '.', ' ');
		return s;
	}

	static std::string escape_html(std::string data) {
		// Simple helper to prevent breaking DOT labels
		return data;
	}
};

} // namespace mir