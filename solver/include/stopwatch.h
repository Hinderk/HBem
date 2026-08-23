
#ifndef __STOPWATCH_H
#define __STOPWATCH_H

#include <chrono>


typedef std::chrono::time_point<std::chrono::steady_clock> watch_t;
typedef std::chrono::microseconds                          delta_t ;


constexpr const delta_t ZERO = std::chrono::microseconds::zero() ;



class StopWatch {

   enum ClockState { eRunning , eStopped , eNone } ;

  public:

     StopWatch( void ) ;
    ~StopWatch( void ) {}
     StopWatch( const StopWatch & ) ;
     StopWatch& operator=( const StopWatch & ) ;

     void Continue( void ) ;
     void Reset( void ) ;
     void Restart( void ) ;
     void Stop( void ) ;
     void Start( void ) ;
     int32_t CycleCount( void ) const ;

     int32_t CpuTime( void ) const ;
     int32_t RealTime( void ) const ;

  private:

     watch_t GetRealTime( void ) const ;  // Specific implementation that
     watch_t GetCPUTime( void ) const ;   // depends on operating system

  private:

     watch_t    StartRealTime ;           // wall clock start time
     watch_t    StopRealTime ;            // wall clock stop time
     watch_t    StartCpuTime ;            // cpu start time
     watch_t    StopCpuTime ;             // cpu stop time
     delta_t    TotalCpuTime ;            // total cpu time spent so far
     delta_t    TotalRealTime ;           // total real time spent so far
     int32_t    CycleCounter ;            // number of starts since reset
     ClockState State ;                   // Current state of clock

}  ;



inline StopWatch::StopWatch( void ) :

  StartRealTime( ZERO ) ,
  StopRealTime( ZERO ) ,
  StartCpuTime( ZERO ) ,
  StopCpuTime( ZERO ) ,
  TotalCpuTime( ZERO ) ,
  TotalRealTime( ZERO ) ,
  CycleCounter( 0 ) ,
  State( eNone ) {}



inline StopWatch::StopWatch( const StopWatch &Timer ) :

  StartRealTime( Timer.StartRealTime ) ,
  StopRealTime( Timer.StopRealTime ) ,
  StartCpuTime( Timer.StartCpuTime ) ,
  StopCpuTime( Timer.StopCpuTime ) ,
  TotalCpuTime( Timer.TotalCpuTime ) ,
  TotalRealTime( Timer.TotalRealTime ) ,
  CycleCounter( Timer.CycleCounter ) ,
  State( Timer.State ) {}



inline StopWatch& StopWatch::operator=( const StopWatch &T0 )
{
  if ( this == &T0 )  return *this ;
  StartRealTime = T0.StartRealTime ;
  StopRealTime = T0.StopRealTime ;
  StartCpuTime = T0.StartCpuTime ;
  StopCpuTime = T0.StopCpuTime ;
  TotalCpuTime = T0.TotalCpuTime ;
  TotalRealTime = T0.TotalRealTime ;
  CycleCounter = T0.CycleCounter ;
  State = T0.State ;
  return *this ;
}



inline void StopWatch::Start( void )
{
  if ( State == eRunning )  return ;
  State = eRunning ;
  TotalCpuTime = ZERO ;
  TotalRealTime = ZERO ;
  StartRealTime = GetRealTime() ;
  StartCpuTime = GetCPUTime() ;
}



inline void StopWatch::Restart( void )
{
  State = eRunning ;
  TotalCpuTime = ZERO ;
  TotalRealTime = ZERO ;
  StartRealTime = GetRealTime() ;
  StartCpuTime = GetCPUTime() ;
}



inline void StopWatch::Reset( void )
{
  State = eNone ;
  TotalCpuTime = ZERO ;
  TotalRealTime = ZERO ;
  CycleCounter = 0 ;
}



inline void StopWatch::Stop( void )
{
  using namespace std::chrono ;
  if ( State == eRunning )
  {
    State = eStopped ;
    StopRealTime = GetRealTime() ;
    StopCpuTime = GetCPUTime() ;
    TotalCpuTime +=
      duration_cast<microseconds> ( StopCpuTime - StartCpuTime ) ;
    TotalRealTime +=
      duration_cast<microseconds> ( StopRealTime - StartRealTime ) ;
    CycleCounter ++ ;
  }
}



inline void StopWatch::Continue( void )
{
  StartRealTime = GetRealTime() ;
  StartCpuTime = GetCPUTime() ;
  State = eRunning ;
}



inline int32_t StopWatch::RealTime( void ) const
{
  using namespace std::chrono ;
  if ( State == eNone )  return 0 ;
  if ( State == eStopped )  return TotalRealTime.count() ;
  delta_t CurrentTime =
    duration_cast<microseconds> ( GetRealTime() - StartRealTime ) ;
  return CurrentTime.count() ;
}



inline int32_t StopWatch::CpuTime( void ) const
{
  using namespace std::chrono ;
  if ( State == eNone )  return 0 ;
  if ( State == eStopped )  return TotalCpuTime.count() ;
  delta_t CurrentTime =
    duration_cast<microseconds> ( GetCPUTime() - StartCpuTime ) ;
  return CurrentTime.count() ;
}



inline watch_t StopWatch::GetRealTime( void ) const
{
  return std::chrono::steady_clock::now() ;
}



inline watch_t StopWatch::GetCPUTime( void ) const
{
  return std::chrono::steady_clock::now() ;
}

#endif // __STOPWATCH_H
