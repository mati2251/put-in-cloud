with Ada.Text_IO; use Ada.Text_IO;
with Ada.Integer_Text_IO; use Ada.Integer_Text_IO;

procedure SemMain is

  protected type Sem is
    entry Down (val: Integer);
    procedure Up (val: Integer);
  private
    entry DownP;
    counter: Integer := 5;
    value: Integer := 0;
  end Sem;

  protected body Sem is
    entry Down (val: Integer) when DownP'Count = 0 is
    begin
      value := val;
      requeue DownP;
    end Down;

    entry DownP when counter >= value is
    begin
      counter := counter - value;
    end DownP;

    procedure Up (val: Integer) is
    begin
      counter := counter + val;
    end Up;
  end Sem;

  semaphore: Sem;

  task type TZ (ID : Character; Value : Integer);
  task body TZ is
    Name : String(1 .. 2);
  begin
    Name(1) := 'T';
    Name(2) := ID;
    
    for I in 1 .. 5 loop 
      semaphore.Down(Value);
      Put_Line(Name & " entered critical section");
      Put_Line("Critical section of " & Name);
      delay 0.5;
      Put_Line(Name & " leaving critical section");
      semaphore.Up(Value);
    end loop;
  end TZ;

  T1 : TZ('1', 1);
  T2 : TZ('2', 2);
  T3 : TZ('3', 3);
  T4 : TZ('4', 4);
  T5 : TZ('5', 5);

begin
  Put_Line("Semaphore");
end SemMain;
