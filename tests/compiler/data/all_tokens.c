int main ()
{
    int x = 10;
    int y = 20;

    if (x < y && x != 0)
    {
        x = x + y;
    }

    while (x > 0)
    {
        x = x - 1;
    }

    return x;
}
