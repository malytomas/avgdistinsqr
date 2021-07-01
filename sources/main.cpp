#include <cage-core/logger.h>
#include <cage-core/math.h>
#include <cage-core/random.h>
#include <cage-core/threadPool.h>
#include <cage-core/concurrent.h>
#include <cage-core/timer.h>

using namespace cage;

Holder<Mutex> mutex = newMutex();
double globalSum = 0;
uint64 globalCount = 0;
uint64 globalMeasurements = 0;

void thrEntry(uint32, uint32)
{
	Holder<Timer> timer = newTimer();
	RandomGenerator gen;
	constexpr uint32 batch = 512;
	real xs1[batch];
	real ys1[batch];
	real xs2[batch];
	real ys2[batch];
	real dst2[batch];
	double sum = 0;
	uint64 batches = 0;
	while (timer->duration() < 1000000) // one second
	{
		for (real &it : xs1)
			it = gen.randomChance(); // random value in range 0 .. 1
		for (real &it : ys1)
			it = gen.randomChance();
		for (real &it : xs2)
			it = gen.randomChance();
		for (real &it : ys2)
			it = gen.randomChance();
		for (uint32 i = 0; i < batch; i++)
			dst2[i] = sqr(xs1[i] - xs2[i]) + sqr(ys1[i] - ys2[i]);
		real s = 0;
		for (const real it : dst2)
			s += sqrt(it).value;
		sum += (s / batch).value;
		batches++;
	}
	{
		ScopeLock lock(mutex);
		globalSum += sum / batches;
		globalCount += 1;
		globalMeasurements += batches * batch;
	}
}

int main(int argc, const char *args[])
{
	Holder<Logger> log = newLogger();
	log->format.bind<logFormatConsole>();
	log->output.bind<logOutputStdOut>();

	try
	{
		Holder<ThreadPool> thrs = newThreadPool();
		thrs->function.bind<&thrEntry>();
		thrs->run();

		const double result = globalSum / globalCount;
		CAGE_LOG(SeverityEnum::Info, "avgdist", stringizer() + "average distance: " + result);
		CAGE_LOG(SeverityEnum::Info, "avgdist", stringizer() + "samples: " + globalMeasurements);

		return 0;
	}
	catch (...)
	{
		detail::logCurrentCaughtException();
	}
	return 1;
}
