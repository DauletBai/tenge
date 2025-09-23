# Tenge Language - Kazakh Operators and Delimiters

**Казахские названия операторов и разделителей**

## 🔧 Операторы (Operators)

| Английское название | Казахское название | Символ | Значение |
|-------------------|-------------------|--------|----------|
| `ASSIGN` | `TAYINDAU` | `=` | тағайындау (назначение) |
| `PLUS` | `KOSU` | `+` | қосу (сложение) |
| `MINUS` | `AZAYTU` | `-` | азайту (вычитание) |
| `MULTIPLY` | `KOBEYTU` | `*` | көбейту (умножение) |
| `DIVIDE` | `BOLU` | `/` | бөлу (деление) |
| `EQUAL` | `TEN` | `==` | тең (равенство) |
| `GREATER` | `ULKEN` | `>` | үлкен (больше) |

## 📝 Разделители (Delimiters)

| Английское название | Казахское название | Символ | Значение |
|-------------------|-------------------|--------|----------|
| `COMMA` | `VIRGUL` | `,` | үтір (запятая) |
| `COLON` | `EKI_NUQTA` | `:` | екі нүкте (двоеточие) |
| `LPAREN` | `SOL_JAI` | `(` | сол жақ (левая скобка) |
| `RPAREN` | `ON_JAI` | `)` | оң жақ (правая скобка) |
| `LBRACKET` | `SOL_KOSHA` | `[` | сол қосша (левая квадратная скобка) |
| `RBRACKET` | `ON_KOSHA` | `]` | оң қосша (правая квадратная скобка) |
| `ARROW` | `OK` | `->` | оқ (стрелка) |

## 📋 Полная таблица изменений

### **Token.go изменения:**

```go
// Старые английские названия
ASSIGN   = "="
PLUS     = "+"
MINUS    = "-"
MULTIPLY = "*"
DIVIDE   = "/"
EQUAL    = "=="
GREATER  = ">"
COMMA    = ","
COLON    = ":"
LPAREN   = "("
RPAREN   = ")"
LBRACKET = "["
RBRACKET = "]"
ARROW    = "->"

// Новые казахские названия
TAYINDAU = "="   // тағайындау (назначение)
KOSU     = "+"   // қосу (сложение)
AZAYTU   = "-"   // азайту (вычитание)
KOBEYTU  = "*"   // көбейту (умножение)
BOLU     = "/"   // бөлу (деление)
TEN      = "=="  // тең (равенство)
ULKEN    = ">"   // үлкен (больше)
VIRGUL   = ","   // үтір (запятая)
EKI_NUQTA = ":" // екі нүкте (двоеточие)
SOL_JAI  = "("   // сол жақ (левая скобка)
ON_JAI   = ")"   // оң жақ (правая скобка)
SOL_KOSHA = "["  // сол қосша (левая квадратная скобка)
ON_KOSHA = "]"   // оң қосша (правая квадратная скобка)
OK       = "->"  // оқ (стрелка)
```

### **Lexer.go изменения:**

```go
// Обновленные case statements
case '=':
    if l.peekChar() == '=' {
        // ...
        tok = token.Token{Type: token.TEN, Literal: literal}
    } else {
        tok = newToken(token.TAYINDAU, l.ch)
    }
case ':':
    tok = newToken(token.EKI_NUQTA, l.ch)
case '(':
    tok = newToken(token.SOL_JAI, l.ch)
case ')':
    tok = newToken(token.ON_JAI, l.ch)
case ',':
    tok = newToken(token.VIRGUL, l.ch)
case '+':
    tok = newToken(token.KOSU, l.ch)
case '-':
    if l.peekChar() == '>' {
        // ...
        tok = token.Token{Type: token.OK, Literal: literal}
    } else {
        tok = newToken(token.AZAYTU, l.ch)
    }
case '*':
    tok = newToken(token.KOBEYTU, l.ch)
case '/':
    tok = newToken(token.BOLU, l.ch)
case '>':
    tok = newToken(token.ULKEN, l.ch)
case '[':
    tok = newToken(token.SOL_KOSHA, l.ch)
case ']':
    tok = newToken(token.ON_KOSHA, l.ch)
```

## 🎯 Примеры использования

### **Математические операции:**
```tenge
jasau a: san = 10
jasau b: san = 5

// Сложение (қосу)
jasau c: san = a KOSU b  // c = 15

// Вычитание (азайту)
jasau d: san = a AZAYTU b  // d = 5

// Умножение (көбейту)
jasau e: san = a KOBEYTU b  // e = 50

// Деление (бөлу)
jasau f: san = a BOLU b  // f = 2
```

### **Сравнения:**
```tenge
eger a TEN b {
    korset "a equals b"
}

eger a ULKEN b {
    korset "a is greater than b"
}
```

### **Структуры данных:**
```tenge
// Массив с казахскими разделителями
jasau numbers: san[] = [1 VIRGUL 2 VIRGUL 3 VIRGUL 4 VIRGUL 5]

// Функция с параметрами
atqar calculate(x: san VIRGUL y: san): san {
    qaytar x KOSU y
}
```

## ✅ Преимущества казахских названий

1. **Локализация** - Полная поддержка казахского языка
2. **Понятность** - Более интуитивные названия для казахских разработчиков
3. **Культурная идентичность** - Сохранение казахской языковой традиции
4. **Образование** - Обучение программированию на родном языке

## 🔄 Миграция

Для обновления существующего кода:

1. **Автоматическая замена** - Используйте поиск и замену в IDE
2. **Поэтапная миграция** - Обновляйте файлы постепенно
3. **Тестирование** - Проверяйте работоспособность после изменений

## 📚 Дополнительные ресурсы

- [Казахская математическая терминология](https://kz.wikipedia.org/wiki/Математика)
- [Документация Tenge Language](README.md)
- [Примеры кода](examples/)

