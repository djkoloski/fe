#pragma once

#include <Fe/Core/Core.h>
#include <Fe/Container/FeString.h>
#include <Makey/Rule.h>

class BuildCommand
{
private:
	const Rule *_rule;
	feString _inputs;
	feString _implicitDependencies;
	feString _orderOnlyDependencies;
	feString _outputs;
	feString _implicitOutputs;

public:
	const Rule *getRule() const;
	void setRule(const Rule *rule);
	const feString &getInputs() const;
	void setInputs(feStringView inputs);
	const feString &getImplicitDependencies() const;
	void setImplicitDependencies(feStringView implicitDependencies);
	const feString &getOrderOnlyDependencies() const;
	void setOrderOnlyDependencies(feStringView orderOnlyDependencies);
	const feString &getOutputs() const;
	void setOutputs(feStringView outputs);
	const feString &getImplicitOutputs() const;
	void setImplicitOutputs(feStringView implicitOutputs);
};