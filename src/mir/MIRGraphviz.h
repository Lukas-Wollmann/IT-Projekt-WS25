#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "MIR.h"

namespace mir {

class MIRGraphviz {
public:
	static std::string generateDOT(const Module &mod) {
		std::stringstream ss;
		ss << "digraph MIR {\n";
		ss << "  node [shape=record, fontname=\"Consolas\"];\n";
		ss << "  rankdir=TB;\n";

		for (const auto &func : mod.funcs) {
			emitFunction(ss, *func);
		}

		ss << "}\n";
		return ss.str();
	}

private:
	static void emitFunction(std::stringstream &ss, const Function &func) {
		ss << "  subgraph cluster_" << func.name << " {\n";
		ss << "    label=\"" << func.name << "\";\n";
		ss << "    color=blue;\n";

		for (const auto &block : func.blocks) {
			emitBlock(ss, func, *block);
		}

		for (const auto &block : func.blocks) {
			if (block->term) {
				emitTerminatorEdges(ss, func, *block, *block->term);
			}
		}

		ss << "  }\n";
	}

	static void emitBlock(std::stringstream &ss, const Function &func, const BasicBlock &block) {
		U8String blockName = getBlockName(func, block.id);

		ss << "    " << blockName << " [label=\"{";
		ss << "Block " << block.id;
		ss << "|";

		for (const auto &instr : block.instrs) {
			ss << escape(printInstr(*instr)) << "\\l";
		}

		if (block.term) {
			ss << "|" << escape(printTerm(*block.term));
		}

		ss << "}\"];\n";
	}

	static void emitTerminatorEdges(std::stringstream &ss, const Function &func,
									const BasicBlock &src, const Term &term) {
		U8String srcName = getBlockName(func, src.id);

		switch (term.kind) {
			case TermKind::Jump: {
				const auto &jmp = static_cast<const Jump &>(term);
				ss << "    " << srcName << " -> " << getBlockName(func, jmp.target) << ";\n";
				break;
			}
			case TermKind::Branch: {
				const auto &br = static_cast<const Branch &>(term);
				ss << "    " << srcName << " -> " << getBlockName(func, br.then)
				   << " [label=\"T\", color=green];\n";
				ss << "    " << srcName << " -> " << getBlockName(func, br.else_)
				   << " [label=\"F\", color=red];\n";
				break;
			}
			case TermKind::Return: break;
		}
	}

	static U8String getBlockName(const Function &func, BlockID id) {
		return func.name + u8"_bb" + std::to_string(id);
	}

	static std::string printInstr(const Instr &instr) {
		std::stringstream ss;
		switch (instr.kind) {
			case InstrKind::LoadFunc: {
				const auto &i = static_cast<const LoadFunc &>(instr);
				// FuncLookup converts a function name to a register
				ss << "%" << i.dest << " = load_func @" << i.funcName;
				break;
			}
			case InstrKind::Call: {
				const auto &i = static_cast<const Call &>(instr);
				ss << "%" << i.dest << " = call %" << i.callee << "(";
				for (size_t a = 0; a < i.args.size(); ++a) {
					ss << "%" << i.args[a] << (a < i.args.size() - 1 ? ", " : "");
				}
				ss << ")";
				break;
			}
			// ... (other cases: IntLit, CharLit, UnitLit, etc.)
			case InstrKind::IntLit: {
				const auto &i = static_cast<const IntLit &>(instr);
				ss << "%" << i.dest << " = int " << i.value;
				break;
			}
			case InstrKind::UnitLit: {
				const auto &i = static_cast<const UnitLit &>(instr);
				ss << "%" << i.dest << " = unit";
				break;
			}
			case InstrKind::Assign: {
				const auto &i = static_cast<const Assign &>(instr);
				ss << "assign %" << i.dest << " %" << i.src;
				break;
			}
			case InstrKind::Load: {
				const auto &i = static_cast<const Load &>(instr);
				ss << "%" << i.dest << " = load %" << i.addr;
				break;
			}
			case InstrKind::BinaryOp: {
				const auto &i = static_cast<const BinaryOp &>(instr);
				ss << "%" << i.dest << " = binop %" << i.left << ", %" << i.right;
				break;
			}
			case InstrKind::SPCreate: {
				const auto &i = static_cast<const SPCreate &>(instr);
				ss << "%" << i.reg << " = sp_create";
				break;
			}
			case InstrKind::SPRetain: {
				const auto &i = static_cast<const SPCreate &>(instr);
				ss << "%" << i.reg << " = sp_retain";
				break;
			}
			case InstrKind::SPRelease: {
				const auto &i = static_cast<const SPRelease &>(instr);
				ss << "release %" << i.reg;
				break;
			}
			default: ss << "unknown_instr"; break;
		}
		return ss.str();
	}

	static std::string printTerm(const Term &term) {
		std::stringstream ss;
		switch (term.kind) {
			case TermKind::Jump: {
				const auto &t = static_cast<const Jump &>(term);
				ss << "jmp bb" << t.target;
				break;
			}
			case TermKind::Branch: {
				const auto &t = static_cast<const Branch &>(term);
				ss << "br %" << t.cond << ", bb" << t.then << ", bb" << t.else_;
				break;
			}
			case TermKind::Return: {
				const auto &t = static_cast<const Return &>(term);
				ss << "ret %" << t.val;
				break;
			}
		}
		return ss.str();
	}

	static std::string escape(const std::string &in) {
		std::string out;
		for (char c : in) {
			if (c == '{' || c == '}' || c == '|' || c == '<' || c == '>') {
				out += '\\';
			}
			out += c;
		}
		return out;
	}
};

}