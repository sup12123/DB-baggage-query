#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <libpq-fe.h>

PGconn *conn = NULL;

void connect_database() {
    const char *params[] = {
        "dbname=customer_profiles",
        NULL
    };

    conn = PQconnectdbParams(params, 1); // Set expand_dbname flag to 1

    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }
}

void add_entry(GtkWidget *widget, gpointer data) {
    const gchar *name = gtk_entry_get_text(GTK_ENTRY(entry1));
    const gchar *age = gtk_entry_get_text(GTK_ENTRY(entry2));
    const gchar *email = gtk_entry_get_text(GTK_ENTRY(entry3));
    const gchar *weight = gtk_entry_get_text(GTK_ENTRY(entry4));
    const gchar *origin = gtk_entry_get_text(GTK_ENTRY(entry5));
    const gchar *destination = gtk_entry_get_text(GTK_ENTRY(entry6));

    char query[256];
    sprintf(query, "INSERT INTO profiles (name, age, email, weight, origin, destination) VALUES ('%s', '%s', '%s', '%s', '%s', '%s')",
            name, age, email, weight, origin, destination);

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Failed to execute query: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    PQclear(res);

    gtk_label_set_text(GTK_LABEL(status_label), "Entry added successfully");
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *grid;
    GtkWidget *label1;
    GtkWidget *label2;
    GtkWidget *label3;
    GtkWidget *label4;
    GtkWidget *label5;
    GtkWidget *label6;
    GtkWidget *add_button;

    // Initialize GTK
    gtk_init(&argc, &argv);

    // Connect to the database
    connect_database();

    // Create the main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Customer Profile Management");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a vertical box container
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create a grid
    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 0);

    // Create labels
    label1 = gtk_label_new("Name:");
    label2 = gtk_label_new("Age:");
    label3 = gtk_label_new("Email:");
    label4 = gtk_label_new("Weight (kg):");
    label5 = gtk_label_new("Origin (IATA):");
    label6 = gtk_label_new("Destination (IATA):");

    // Attach labels to the grid
    gtk_grid_attach(GTK_GRID(grid), label1, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), label2, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), label3, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), label4, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), label5, 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), label6, 0, 5, 1, 1);

    // Create entry fields
    entry1 = gtk_entry_new();
    entry2 = gtk_entry_new();
    entry3 = gtk_entry_new();
    entry4 = gtk_entry_new();
    entry5 = gtk_entry_new();
    entry6 = gtk_entry_new();

    // Attach entry fields to the grid
    gtk_grid_attach(GTK_GRID(grid), entry1, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry2, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry3, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry4, 1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry5, 1, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry6, 1, 5, 1, 1);

    // Create the Add button
    add_button = gtk_button_new_with_label("Add");
    g_signal_connect(add_button, "clicked", G_CALLBACK(add_entry), NULL);
    gtk_grid_attach(GTK_GRID(grid), add_button, 1, 6, 1, 1);

    // Create the status label
    status_label = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(vbox), status_label, FALSE, FALSE, 0);

    // Show all widgets
    gtk_widget_show_all(window);

    // Start the GTK main loop
    gtk_main();

    // Cleanup and exit
    PQfinish(conn);

    return 0;
}
