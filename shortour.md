# Немного о потрохах

В данной реализации память выделяется блочно. То есть минимальный размер непустого вектора — это размер одного такого блока.
<p align="center">
    <img src="https://i.ibb.co/fNms3kB/memoryvec.png" alt="drawing" width="570" align="center"/>
</p>

- Длина вектора — это количество элементов в векторе.
- Размер вектора — это размер выделенной памяти. 

Стоит отметить, что в этой реализации вектор не уменьшается в ёмкости(размера). Для этого существует специальный интерфейс. 

...

# Вектор и Срезы

Существуют две основные сущности: это сам _вектор_ и _срез_.
- Срез — это указатель на вектор, ссылающийся на определённый участок из него. ~~При этом срез не может изменять исходный вектор, ему доступно только чтение, благодаря чему можно предотвратить несанкционированный доступ памяти на уровне типов. Так, например, чтобы вместо передачи в функцию указателя на вектор вы можете передать срез, явно говоря о том, что предоставляете только чтение.~~

<p align="center">
    <img src="https://i.ibb.co/ZXM9FYw/vecslice.png" alt="drawing" width="570" align="center"/>
</p>

## Вектор

Тип вектора — это [`sbvector`](https://github.com/celtrecium/libsbvector/blob/71ee11c7e5e624f2d672eebb1306b9d97e2dff20/include/sbvector.h#L46). У него есть одно публичное поле: `length`(длина вектора).
```c
typedef struct sbvector
{
  ...
  size_t length;
} sbvector_t;
```

**Создание**

Для создания вектора можно использовать функцию [`sbvector`](https://github.com/celtrecium/libsbvector/blob/71ee11c7e5e624f2d672eebb1306b9d97e2dff20/include/sbvector.h#L65), которая принимает в качестве аргумента размер типа хранимых элементов и возвращает инициализированную структуру. Вот пример:
```c
// sbvector_t sbvector (size_t typesize)
sbvector_t vect = sbvector(sizeof(int)); 
```
Дефолнтный размер блока из коробки составляет [32 элементов указанного типа](https://github.com/celtrecium/libsbvector/blob/3f967555b25eaf664f41caa8c7ea96196435de3a/include/sbvector.h#L44). А длина устанавливается, конечно, на ноль. 

Также для создания вектора можно использовать массив.
```c
// sbvector_t sbvector_from_array (const void *array, size_t array_size, size_t type_size);
int array[] = { 1, 2, 3, 4, 5, 6, 7 };
sbvector_t vect = sbvector_from_array(array, 7, sizeof(int));
```

<br>

**Получение элементов и установление новых значений**

Теперь, когда вы можете создавать векторы, необходимо получать доступ к их элементам. Для этого существуют макросы [`sbv_get`](https://github.com/celtrecium/libsbvector/blob/3f967555b25eaf664f41caa8c7ea96196435de3a/include/sbvector.h#L128) и [`sbv_set`](https://github.com/celtrecium/libsbvector/blob/3f967555b25eaf664f41caa8c7ea96196435de3a/include/sbvector.h#L129).
```c
// sbv_get(sbv, type, index)
*sbv_get(&vect, int, 1); // 2 
```
Здесь в качестве первого аргумента идёт указатель на вектор, затем тип получаемого элемента и в конце сам индекс. 

А чтобы установить значение элементу используем макрос `sbv_set`, который в качестве первого аргумента получает указатель на вектор, затем типа элемента, индекс и само значение. 
```c
// sbv_set(sbv, type, index, data)
sbv_set(&vect, int, 0, 0); // vect[0] = 0
```

<br>

**Push и Pop**

Для добавления новых элементов в конец вектора существуют два интерфейса: `sbv_push` и `sbv_pop`. 
```c
// sbv_push(sbv, type, data)
sbv_push(&vect, int, 8);
// bool sbv_pop (sbvector_t *sbv);
sbv_pop(&vect, int);
```
Стоит обратить внимание, что при использовании `sbv_pop` память следует освобождать в ручном режиме, если это, конечно, требуется.

---

**Соглашение по обработке ошибок**

В libsbvector для проверки корректности завершения функции, используется соглашение, при котором функция будет возвращать тип `bool`, где `true` означает, что всё выполнилось корректно, а `false` сообщит о том, что произошла ошибка.  

---

<br>

**Очищение и освобождение вектора**

Для того, чтобы удалить все элементы в векторе(удаление тут означает, что длина вектора станет равна нулю) `sbv_clear`. 
```c
// bool sbv_clear (sbvector_t *sbv)
sbv_clear(&vect);
```
Чтобы же освободить сами данные, хранящиеся в векторе, существует функция `sbv_free`.
```c
//  bool sbv_free (sbvector_t *sbv)
sbv_free(&vect);
``` 
Как и везде ранее, освобождение памяти происходит в ручном режиме.

<br>

**Дополнительные интерфейсы**

```c
// Копирует содержимое одного вектора в другой.
bool sbv_copy (sbvector_t *dest, sbvector_t *src);

// Обрезает размер ёмкости вектора до минимально возможного. 
bool sbv_crop_capacity (sbvector_t *sbv);

// Установление размера вектора. 
bool sbv_resize (sbvector_t *sbv, size_t new_size);

// Установление размера выделяемого блока памяти.
bool sbv_set_blocksize (sbvector_t *sbv, size_t new_block_size);
```

<br>

## Срез

Тип среза — это `sbslice_t`; Структура содержит два публичных поля: длину и указатель на исходный вектор.
```c
typedef struct sbslice
{
  ...
  size_t length;
  sbvector_t *vector;
} sbslice_t;
```

**Создание**
```c
// sbslice_t sbslice (sbvector_t *sbv, size_t begin, size_t end);
sbslice_t s = sbslice(&vect, 0, 3);
```

<br>

**Получение доступа к элементам в срезе**
```c
// sbslice_get(sbsl, type, index) 
sbslice_get(&slice, int, 2);

// sbslice_set(sbsl, type, index, data)
sbslice_set(&slice, 2, 6);
```

## Послесловия 

Для более чёткого представление обо всех возможностей данной библиотеки советуем заглянуть в исходный код, либо в документацию.  
