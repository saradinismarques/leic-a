!! external builtin functions
int ?argc()
string ?argv(int n)
int ?atoi(string s)
!! external user functions
int ?factorial(int n)

!! the main function
int *fir() -> 0
@ { int f = 1; }
{
  writeln 'Teste para a função factorial';
  if argc() == 2 then f = atoi(argv(1));
  writeln fir, '! = ', factorial(f);
}
>> { (* nothing to do, really *) }