
#include "../TestConfig.hpp"

class TestBranch : public TestFixture
{
};

TEST_F(TestBranch, GENERAL_BRANCH_PAGECROSS)
{
	asse.Assemble(R"(
		bcc -13
	)");

	clearCycles();
	// pc is 2, and the offset is
	// -13 so offset + pc = -11
	// pc is u16 so it wraps to $0000 - 11 = $fff5
	ASSERT_EQ(console.GetCpu().PC(), 0xfff5);
	// page crossing
	ASSERT_EQ(console.GetCpu().GetCycles(), 3);
}

TEST_F(TestBranch, BCC_BRANCH)
{
	asse.Assemble(R"(
		bcc 2
	)");
	clearCycles();

	ASSERT_EQ(console.GetCpu().PC(), 4);
	// no page crossing
	ASSERT_EQ(console.GetCpu().GetCycles(), 2);
}

TEST_F(TestBranch, BCC_NOBRANCH)
{
	asse.Assemble(R"(
		sec
		bcc 2
	)");

	run_instruction();
	run_instruction();

	// no branching
	ASSERT_EQ(console.GetCpu().PC(), 3);
}

TEST_F(TestBranch, BCS_BRANCH)
{
	asse.Assemble(R"(
		sec
		bcs 2
	)");

	run_instruction();
	clearCycles();

	ASSERT_EQ(console.GetCpu().PC(), 5);
	// no page crossing
	ASSERT_EQ(console.GetCpu().GetCycles(), 2);
}

TEST_F(TestBranch, BCS_NOBRANCH)
{
	asse.Assemble(R"(
			bcs 2
		)");

	clearCycles();

	// no branching
	ASSERT_EQ(console.GetCpu().PC(), 2);
	ASSERT_EQ(console.GetCpu().GetCycles(), 1);
}

TEST_F(TestBranch, BEQ_BRANCH)
{
	console.GetCpu().SetP(Emu::ProcessorStatus::Flags::Z);
	asse.Assemble(R"(
			beq 2
		)");

	clearCycles();

	ASSERT_EQ(console.GetCpu().PC(), 4);
	// no page crossing
	ASSERT_EQ(console.GetCpu().GetCycles(), 2);
}

TEST_F(TestBranch, BEQ_NOBRANCH)
{
	asse.Assemble(R"(
			beq 2
		)");

	clearCycles();

	// no branching
	ASSERT_EQ(console.GetCpu().PC(), 2);
	ASSERT_EQ(console.GetCpu().GetCycles(), 1);
}

TEST_F(TestBranch, BMI_BRANCH)
{
	console.GetCpu().SetP(Emu::ProcessorStatus::Flags::N);
	asse.Assemble(R"(
			bmi 2
		)");

	clearCycles();

	ASSERT_EQ(console.GetCpu().PC(), 4);
	// no page crossing
	ASSERT_EQ(console.GetCpu().GetCycles(), 2);
}

TEST_F(TestBranch, BMI_NOBRANCH)
{
	asse.Assemble(R"(
			bmi 2
		)");

	clearCycles();

	// no branching
	ASSERT_EQ(console.GetCpu().PC(), 2);
	ASSERT_EQ(console.GetCpu().GetCycles(), 1);
}

TEST_F(TestBranch, BNE_BRANCH)
{
	asse.Assemble(R"(
			bne 2
		)");

	clearCycles();

	ASSERT_EQ(console.GetCpu().PC(), 4);
	// no page crossing
	ASSERT_EQ(console.GetCpu().GetCycles(), 2);
}

TEST_F(TestBranch, BNE_NOBRANCH)
{
	console.GetCpu().SetP(Emu::ProcessorStatus::Flags::Z);
	asse.Assemble(R"(
			bne 2
		)");

	clearCycles();

	// no branching
	ASSERT_EQ(console.GetCpu().PC(), 2);
	ASSERT_EQ(console.GetCpu().GetCycles(), 1);
}

TEST_F(TestBranch, BPL_BRANCH)
{
	asse.Assemble(R"(
			bpl 2
		)");

	clearCycles();

	ASSERT_EQ(console.GetCpu().PC(), 4);
	// no page crossing
	ASSERT_EQ(console.GetCpu().GetCycles(), 2);
}

TEST_F(TestBranch, BPL_NOBRANCH)
{
	console.GetCpu().SetP(Emu::ProcessorStatus::Flags::N);
	asse.Assemble(R"(
			bpl 2
		)");
	clearCycles();

	// no branching
	ASSERT_EQ(console.GetCpu().PC(), 2);
	ASSERT_EQ(console.GetCpu().GetCycles(), 1);
}

TEST_F(TestBranch, BVC_BRANCH)
{
	asse.Assemble(R"(
			bvc 2
		)");

	clearCycles();

	ASSERT_EQ(console.GetCpu().PC(), 4);
	// no page crossing
	ASSERT_EQ(console.GetCpu().GetCycles(), 2);
}

TEST_F(TestBranch, BVC_NOBRANCH)
{
	console.GetCpu().SetP(Emu::ProcessorStatus::Flags::V);
	asse.Assemble(R"(
			bvc 2
		)");

	clearCycles();

	// no branching
	ASSERT_EQ(console.GetCpu().PC(), 2);
	ASSERT_EQ(console.GetCpu().GetCycles(), 1);
}

TEST_F(TestBranch, BVS_BRANCH)
{
	console.GetCpu().SetP(Emu::ProcessorStatus::Flags::V);
	asse.Assemble(R"(
			bvs 2
		)");

	clearCycles();

	ASSERT_EQ(console.GetCpu().PC(), 4);
	// no page crossing
	ASSERT_EQ(console.GetCpu().GetCycles(), 2);
}

TEST_F(TestBranch, BVS_NOBRANCH)
{
	asse.Assemble(R"(
			bvs 2
		)");

	clearCycles();

	// no branching
	ASSERT_EQ(console.GetCpu().PC(), 2);
	ASSERT_EQ(console.GetCpu().GetCycles(), 1);
}
