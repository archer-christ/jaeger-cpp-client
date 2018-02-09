#include <jaegertracing/Tracer.h>

#include <chrono>
#include <thread>

using std::string;

const string cfgstr {R"endyaml(
disabled: false
sampler:
    type: const
    param: 1
reporter:
    queueSize: 100
    bufferFlushInterval: 10
    logSpans: false
    localAgentHostPort: 127.0.0.1:6831
headers:
    jaegerDebugHeader: debug-id
    jaegerBaggageHeader: baggage
    TraceContextHeaderName: trace-id
    traceBaggageHeaderPrefix: "testctx-"
baggage_restrictions:
    denyBaggageOnInitializationFailure: false
    hostPort: 127.0.0.1:5778
    refreshInterval: 60
)endyaml"};

int main(int argc, char *argv[])
{
	const auto config = jaegertracing::Config::parse(YAML::Load(cfgstr));
	auto tracer = jaegertracing::Tracer::make("shortlived", config);

	string opname("shortlived");
	if (argc > 1)
	{
		opname += "-";
		opname += argv[1];
	}
	auto span = tracer->StartSpan(opname);
	span->Finish();
	span.reset();
	/* This should ensure that all spans reach the server */
	//static_cast<jaegertracing::Tracer*>(tracer.get())->flush();
	tracer->Close();
}
