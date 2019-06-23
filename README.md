**a simple http client with routing capabilities
**

```
    NetworkManager manager;

    manager.get("https://reqres.in/api/users?page=2")->subcribe([](NetworkResponse *res)
    {
        qDebug()<<res->json();
    });
```