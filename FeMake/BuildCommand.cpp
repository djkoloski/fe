#include <FeMake/BuildCommand.h>

const Rule *BuildCommand::getRule() const
{
	return _rule;
}
void BuildCommand::setRule(const Rule *rule)
{
	_rule = rule;
}
const feString &BuildCommand::getInputs() const
{
	return _inputs;
}
void BuildCommand::setInputs(feStringView inputs)
{
	_inputs = inputs;
}
const feString &BuildCommand::getImplicitDependencies() const
{
	return _implicitDependencies;
}
void BuildCommand::setImplicitDependencies(feStringView implicitDependencies)
{
	_implicitDependencies = implicitDependencies;
}
const feString &BuildCommand::getOrderOnlyDependencies() const
{
	return _orderOnlyDependencies;
}
void BuildCommand::setOrderOnlyDependencies(feStringView orderOnlyDependencies)
{
	_orderOnlyDependencies = orderOnlyDependencies;
}
const feString &BuildCommand::getOutputs() const
{
	return _outputs;
}
void BuildCommand::setOutputs(feStringView outputs)
{
	_outputs = outputs;
}
const feString &BuildCommand::getImplicitOutputs() const
{
	return _implicitOutputs;
}
void BuildCommand::setImplicitOutputs(feStringView implicitOutputs)
{
	_implicitOutputs = implicitOutputs;
}