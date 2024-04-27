#include "Disassembler.h"


void Dissassemble(const void* pStartAddress, const void* pAddress, size_t size, std::map<uint64_t, InstructionOperand>& insOp)
{
	ZydisDecoder decoder;
	const uint8_t* pBuffer = reinterpret_cast<const uint8_t*>(pAddress);
	size_t length = 0;

	ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64);

	InstructionOperand op;

	while (ZYAN_SUCCESS(ZydisDecoderDecodeFull(&decoder, pBuffer + length, size - length, &op.Instruction, op.Operands)))
	{
		uint64_t offset = reinterpret_cast<uint64_t>(pBuffer + length);
		length += op.Instruction.length;

		if (insOp.find(offset) != insOp.end())
			break;

		insOp.insert(std::pair<uint64_t, InstructionOperand>(offset, op));

		memset(op.Operands, 0, sizeof(op.Operands));

		if (op.Instruction.mnemonic == ZYDIS_MNEMONIC_JB ||
			op.Instruction.mnemonic == ZYDIS_MNEMONIC_JBE ||
			op.Instruction.mnemonic == ZYDIS_MNEMONIC_JCXZ ||
			op.Instruction.mnemonic == ZYDIS_MNEMONIC_JECXZ ||
			op.Instruction.mnemonic == ZYDIS_MNEMONIC_JKNZD ||
			op.Instruction.mnemonic == ZYDIS_MNEMONIC_JKZD ||
			op.Instruction.mnemonic == ZYDIS_MNEMONIC_JL ||
			op.Instruction.mnemonic == ZYDIS_MNEMONIC_JLE ||
			op.Instruction.mnemonic == ZYDIS_MNEMONIC_JMP ||
			op.Instruction.mnemonic == ZYDIS_MNEMONIC_JNB ||
			op.Instruction.mnemonic == ZYDIS_MNEMONIC_JNBE ||
			op.Instruction.mnemonic == ZYDIS_MNEMONIC_JNLE ||
			op.Instruction.mnemonic == ZYDIS_MNEMONIC_JNO ||
			op.Instruction.mnemonic == ZYDIS_MNEMONIC_JNP ||
			op.Instruction.mnemonic == ZYDIS_MNEMONIC_JNS ||
			op.Instruction.mnemonic == ZYDIS_MNEMONIC_JNZ ||
			op.Instruction.mnemonic == ZYDIS_MNEMONIC_JO ||
			op.Instruction.mnemonic == ZYDIS_MNEMONIC_JP ||
			op.Instruction.mnemonic == ZYDIS_MNEMONIC_JRCXZ ||
			op.Instruction.mnemonic == ZYDIS_MNEMONIC_JS ||
			op.Instruction.mnemonic == ZYDIS_MNEMONIC_JZ)
		{
			auto newOffset = (pBuffer + length) + op.Instruction.raw.imm->value.s;

			Dissassemble(pStartAddress, newOffset, size, insOp);
		}

		if (op.Instruction.mnemonic == ZYDIS_MNEMONIC_RET)
			break;

		if (length >= size)
			break;
	}
}

void Dissassemble(const void* pAddress, size_t size, std::map<uint64_t, InstructionOperand>& instructions)
{
	Dissassemble(pAddress, pAddress, size, instructions);

	uint64_t nextOffset = reinterpret_cast<uint64_t>(pAddress);

	auto it4 = instructions.find(nextOffset);

	if (it4 == instructions.end())
		return;

	instructions.erase(instructions.begin(), it4);
}