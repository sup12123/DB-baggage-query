#include <gtk/gtk.h>
#include <libpq-fe.h>

GtkWidget *entry1;
GtkWidget *entry2;
GtkWidget *entry3;
GtkWidget *entry4;
GtkWidget *status_label;
PGconn *conn = NULL;

void connect_database(GtkWidget *widget, gpointer data) {
    const gchar *dbname = gtk_entry_get_text(GTK_ENTRY(entry1));
    const gchar *username = gtk_entry_get_text(GTK_ENTRY(entry2));
    const gchar *password = gtk_entry_get_text(GTK_ENTRY(entry3));
    const gchar *host = gtk_entry_get_text(GTK_ENTRY(entry4));

    // Construct the connection string
    gchar *conninfo = g_strdup_printf("dbname=%s user=%s password=%s host=%s", dbname, username, password, host);
    
    // Connect to the database
    conn = PQconnectdb(conninfo);
    
    // Check if the connection was successful
    if (PQstatus(conn) != CONNECTION_OK) {
        gchar *error_message = g_strdup_printf("Connection to database failed: %s", PQerrorMessage(conn));
        gtk_label_set_text(GTK_LABEL(status_label), error_message);
        g_free(error_message);
        PQfinish(conn);
        conn = NULL;
        g_free(conninfo);
        return;
    }
    
    gtk_label_set_text(GTK_LABEL(status_label), "Connected to database");
    
    g_free(conninfo);
}

void add_entry(GtkWidget *widget, gpointer data) {
    if (!conn) {
        gtk_label_set_text(GTK_LABEL(status_label), "Not connected to database");
        return;
    }
    
    const gchar *name = gtk_entry_get_text(GTK_ENTRY(entry1));
    const gchar *email = gtk_entry_get_text(GTK_ENTRY(entry2));
    const gchar *phone = gtk_entry_get_text(GTK_ENTRY(entry3));
    const gchar *weight = gtk_entry_get_text(GTK_ENTRY(entry4));
    const gchar *origin = gtk_entry_get_text(GTK_ENTRY(entry5));
    const gchar *destination = gtk_entry_get_text(GTK_ENTRY(entry6));
    
    // Begin a database transaction
    PGresult *res = PQexec(conn, "BEGIN");
    
    // Check if the transaction was successful
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        gchar *error_message = g_strdup_printf("Failed to begin database transaction: %s", PQerrorMessage(conn));
        gtk_label_set_text(GTK_LABEL(status_label), error_message);
        g_free(error_message);
        PQclear(res);
        return;
    }
    
    // Prepare the SQL statements
    gchar *sql1 = g_strdup_printf("INSERT INTO customer_profile (name, email, phone) VALUES ('%s', '%s', '%s') RETURNING id", name, email, phone);
    gchar *sql2 = g_strdup_printf("INSERT INTO bags (weight, origin, destination, profile_id) VALUES (%s, '%s', '%s', $1)", weight, origin, destination);
    
    // Execute the first SQL statement and get the profile_id
    res = PQexecParams(conn, sql1, 0, NULL, NULL, NULL, NULL, 1);
    
    // Check if the execution was successful
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        gchar *error_message = g_strdup_printf("Failed to execute SQL statement: %s", PQerrorMessage(conn));
        gtk_label_set_text(GTK_LABEL(status_label), error_message);
        g_free(error_message);
        PQclear(res);
        g_free(sql1);
        g_free(sql2);
        return;
    }
    
    // Get the profile_id from the result
    const gchar *profile_id = PQgetvalue(res, 0, 0);
    
    // Execute the second SQL statement with the retrieved profile_id
    const char *params[1] = { profile_id };
    res = PQexecParams(conn, sql2, 1, NULL, params, NULL, NULL, 0);
    
    // Check if the execution was successful
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        gchar *error_message = g_strdup_printf("Failed to execute SQL statement: %s", PQerrorMessage(conn));
        gtk_label_set_text(GTK_LABEL(status_label), error_message);
        g_free(error_message);
        PQclear(res);
        g_free(sql1);
        g_free(sql2);
        return;
    }
    
    // Commit the database transaction
    res = PQexec(conn, "COMMIT");
    
    // Check if the transaction was successful
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        gchar *error_message = g_strdup_printf("Failed to commit database transaction: %s", PQerrorMessage(conn));
        gtk_label_set_text(GTK_LABEL(status_label), error_message);
        g_free(error_message);
        PQclear(res);
        g_free(sql1);
        g_free(sql2);
        return;
    }
    
    gtk_label_set_text(GTK_LABEL(status_label), "Entry added to the database");
    
    PQclear(res);
    g_free(sql1);
    g_free(sql2);
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

    // Create the main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Customer Profile");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a vertical box
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create a grid
    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_box_pack_start(GTK_BOX(vbox), grid, FALSE, FALSE, 0);

    // Create labels
    label1 = gtk_label_new("Name:");
    label2 = gtk_label_new("Email:");
    label3 = gtk_label_new("Phone:");
    label4 = gtk_label_new("Weight (kg):");
    label5 = gtk_label_new("Origin:");
    label6 = gtk_label_new("Destination:");

    // Add labels to the grid
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

    // Add entry fields to the grid
    gtk_grid_attach(GTK_GRID(grid), entry1, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry2, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry3, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry4, 1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry5, 1, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry6, 1, 5, 1, 1);

    // Create a button
    add_button = gtk_button_new_with_label("Add Entry");
    gtk_box_pack_start(GTK_BOX(vbox), add_button, FALSE, FALSE, 0);

    // Create a status label
    status_label = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(vbox), status_label, FALSE, FALSE, 0);

    // Connect signals
    g_signal_connect(add_button, "clicked", G_CALLBACK(add_entry), NULL);

    // Show all widgets
    gtk_widget_show_all(window);

    // Run the GTK main loop
    gtk_main();

    // Cleanup
    if (conn) {
        PQfinish(conn);
    }

    return 0;
}
