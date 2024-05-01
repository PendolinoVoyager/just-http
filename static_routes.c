struct route {
    char    *uri;
    void    *file;
};
struct route ROUTES[] = {
    {
        .uri = "/",
        .file = "index.html"
    },
};