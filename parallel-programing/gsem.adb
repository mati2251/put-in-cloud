with Ada.Text_IO; use Ada.Text_IO;
with Ada.Integer_Text_IO; use Ada.Integer_Text_IO;

procedure Sem is 

  protected type Gsem is
    entry Down;
    procedure Up;
  private
    counter: Integer := 1;
  end Gsem;

  protected body Gsem is
    entry Down when counter > 0 is
    begin
      counter := counter - 1;
    end Down;

    procedure Up is
    begin
      counter := counter + 1;
    end Up;
  end Gsem;

  generalSemaphore: Gsem;

  task type TZ (ID : Character);
  task body TZ is
    Name : String(1 .. 2);
  begin
    Name(1) := 'T';
    Name(2) := ID;
    for I in 1 .. 5 loop 
      generalSemaphore.Down;
      Put_Line(Name & " entered critical section");
      Put_Line("Critical section of " & Name);
      delay 0.5;
      Put_Line(Name & " leaving critical section");
      generalSemaphore.Up;
    end loop;
  end TZ;

  T1 : TZ('1');
  T2 : TZ('2');
  T3 : TZ('3');
  T4 : TZ('4');
  T5 : TZ('5');

begin
  Put_Line("Semaphore");
end Sem;
