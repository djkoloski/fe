#include <Fe/Core/Core.h>

#include <Test/Test.h>

void printClassInfo(feObject *obj)
{
	const auto &meta = *obj->getMetaObject();

	for (auto i = feUInt(0); i < meta.getMetaFieldCount(); ++i)
	{
		const auto &field = meta.getMetaFields()[i];

		FE_PRINT("Field %i: size %i, offset %i\n", i, field.getSize(), field.getOffset());
	}
}

feInt feMain(feInt argc, const feRawString *argv)
{
	FE_PRINT("Hello world!\n");

	auto *testClass = new TestClass();

	printClassInfo(testClass);

	delete testClass;

	return 0;
}