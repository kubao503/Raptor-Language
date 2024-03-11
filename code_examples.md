### Typy danych i operacje
```
int i = 3 + 2 * 4;
float f = 2 * 2.0;
bool b = true or 1 == 1 and true != true;

print(i);
print(f);
print(b);
```
Output:
```
11
4.0
false
```

### Stałe
```
const float pi = 3.14;
```

### Typ znakowy
```
str w = "Hello\n\"world\"";
print(w);

str v = "Hello" % " " % "wo";
v += "rld";
print(v);
```
Output:
```
Hello
"world"
Hello world
```

### Komentarze
```
int i = 1; # Single-line comment
```

### Instrukcja warunkowa if
```
if 2 != 3 {
    print(3);
}
```

### Instrukcja pętli
```
int i = 0;
while true {
    i += 1;
    if i == 10 {
        break;
    }
}
print(i);
```
Output:
```
10
```

### Struktura
```
struct Point {
    int x;
    int y;
}

Point p = #TODO
p.x = 7;
p.y = 1;
```

### Rekord wariantowy
```
variant Any(int, float);

Any<int> a = 5;
Any<float> b = 2.0;
```

### Funkcje
```
int add_one(int num) {
    return num + 1;
}

void add_one_ref(ref int num) {
    num += 1;
}

int i = 3;
int res = add_one(i);   # Pass by value
print(res);

add_one_ref(ref i);     # Pass by reference
print(i);
```
Output:
```
4
4
```

### Przykrywanie zmiennych
```

int i = 3;

```

### Rekurencja
```
void count_down_to_zero(int i) {
    print(i);
    if i == 0 {
        return;
    }
    count_down_to_zero(i - 1);
}

count_down_to_zero(3);
```
Output:
```
3
2
1
0
```

## Błędy

### Użycie zmiennej niezainicjowanej
```
print(i)
```

### Próba przypisania do stałej
```
const int c = 5;
c = 10;
```

## Konwersja typów

## Gramatyka
PROGRAM =           STMTS;
STMTS =             STMTS STMT
                  | ϵ
