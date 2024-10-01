Philosopher[] philosophers = [new (), new (), new (), new (), new ()];

Fork[] forks = [new(), new(), new(), new()];

foreach (var philosopher in philosophers)
{
    var thread = new Thread(() =>
    {
        while (true)
        {
            var state = philosopher.State();

            if (state == PhilosopherState.Thinking)
            {
                philosopher.Hunger -= 1;
            }
            else
            {
                philosopher.Hunger += 1;
            }

            if (philosopher.Hunger == 0)
            {
                break;
            }
            
            Thread.Sleep(100);
        }
    });
    
    thread.Start();
}

var keepGoing = true;

while (keepGoing)
{
    var phils = philosophers.OrderBy(philosopher => philosopher.Hunger);
    
    var hungriest = phils.Take(2).ToList();
    
    foreach(var philosopher in philosophers)
    {
        if (philosopher.Hunger >= 100) philosopher.TakeForks();
        else
            foreach (var _ in hungriest.Where(hungry => philosopher.Hunger > hungry.Hunger))
            {
                philosopher.TakeForks();
            }
    }

    foreach (var fork in forks)
    {
        foreach (var hungry in hungriest.Where(hungry => fork.Available && hungry.NeedsFork()))
        {
            hungry.GiveFork(fork);
        }
    }

    for (var i = 0; i < philosophers.Length; i++)
    {
        var hungry = philosophers[i].Hunger;
        Console.WriteLine("Hunger level of philosopher " + (i + 1) + " is " + hungry);

        keepGoing &= hungry != 0;
    }
    
    Thread.Sleep(100);
    if (keepGoing) Console.Clear();
}

internal class Fork
{
    public bool Available { get; set; } = true;
}

internal enum PhilosopherState
{
    Thinking,
    Eating,
}

internal class Philosopher
{
    private Fork? _leftFork;
    private Fork? _rightFork;
    internal int Hunger = Random.Shared.Next(50, 100);

    public bool NeedsFork()
    {
        return _leftFork is null | _rightFork is null;
    }

    public void GiveFork(Fork fork)
    {
        fork.Available = false;
        if (_rightFork is null)
        {
            _rightFork = fork;
        } else if (_leftFork is null)
        {
            _leftFork = fork;
        }
    }

    public int ForkCount()
    {
        return (_leftFork is null).CompareTo(false) + (_rightFork is null).CompareTo(false);
    }

    public void TakeForks()
    {
        if (_rightFork != null)
        {
            _rightFork.Available = true;
            _rightFork = null;
        }

        if (_leftFork == null) return;
        _leftFork.Available = true;
        _leftFork = null;
    }

    public PhilosopherState State()
    {
        if (_leftFork == null || _rightFork == null)
        {
            return PhilosopherState.Thinking;
        }
        
        return PhilosopherState.Eating;
    }
}
