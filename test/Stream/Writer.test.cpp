#include "Stream/Writer.h"
#include "Stream/DynamicMemoryWriter.h"
#include <gtest/gtest.h>
#include <vector>
#include <type_traits>

// Note: Writer is an abstract class, so it can not be tested directly.
// DynamicMemoryWriter will be used to test the template methods in Writer


TEST(Writer, CanSerializeVectorOfTrivialStruct) {
	struct TrivialStruct {
		int field1;
		int field2;

		bool operator==(TrivialStruct other) const {
			return (field1 == other.field1) && (field2 == other.field2);
		}
	};

	// Test values
	const std::vector<TrivialStruct> writeValue{{10, 1}, {20, 2}};
	std::vector<TrivialStruct> readValue(writeValue.size());

	// Write value
	Stream::DynamicMemoryWriter writer;
	EXPECT_NO_THROW(writer.Write(writeValue));

	// Read value
	auto reader = writer.GetReader();
	EXPECT_NO_THROW(reader.Read(readValue));
	EXPECT_EQ(writeValue, readValue);
}

TEST(Writer, CanSerializeNonTrivialType) {
	struct NonTrivialStruct {
		virtual void virtualFunction() {};

		void Write(Stream::Writer& writer) {
			writer.Write(testInteger);
		}

		int testInteger = 1;
	};

	ASSERT_FALSE(std::is_trivially_copyable<NonTrivialStruct>::value);

	NonTrivialStruct nonTrivialStruct;

	// Write value
	Stream::DynamicMemoryWriter writer;
	EXPECT_NO_THROW(writer.Write(nonTrivialStruct));

	// Read value
	int destination;
	auto reader = writer.GetReader();
	reader.Read(destination);
	EXPECT_EQ(destination, nonTrivialStruct.testInteger);
}
