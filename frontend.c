#include <gtk/gtk.h>
#include <libpq-fe.h>

PGconn *conn;

// Function to execute a PostgreSQL query
PGresult* executeQuery(const char *query) {
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("Query execution failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }
    return res;
}

// Callback function for the "Add Bags" button
void addBagsClicked(GtkButton *button, gpointer data) {
    // Add your code here for adding bags to the selected user
    g_print("Add Bags clicked!\n");
}

// Callback function for the "Create Ticket" button
void createTicketClicked(GtkButton *button, gpointer data) {
    // Add your code here for creating a new ticket/reservation for the selected user
    g_print("Create Ticket clicked!\n");
}

// Callback function for the user selection
void userSelected(GtkComboBox *combo, gpointer data) {
    gchar *selectedUser = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo));
    if (selectedUser != NULL) {
        // Add your code here for retrieving the user details or performing any other actions
        g_print("Selected User: %s\n", selectedUser);
        g_free(selectedUser);
    }
}

// Function to populate the user combo box with data from the database
void populateUserComboBox(GtkComboBoxText *combo) {
    const char *query = "SELECT username FROM users";
    PGresult *res = executeQuery(query);
    if (res == NULL) {
        // Handle the error
        return;
    }

    int numRows = PQntuples(res);
    for (int i = 0; i < numRows; i++) {
        const char *username = PQgetvalue(res, i, 0);
        gtk_combo_box_text_append_text(combo, username);
    }

    PQclear(res);
}

// Main function
int main(int argc, char *argv[]) {
    // Initialize GTK
    gtk_init(&argc, &argv);

    // Create the main window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "User Selection");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a vertical box container
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create the user selection combo box
    GtkWidget *combo = gtk_combo_box_text_new();
    populateUserComboBox(GTK_COMBO_BOX_TEXT(combo));
    g_signal_connect(combo, "changed", G_CALLBACK(userSelected), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), combo, FALSE, FALSE, 0);

    // Create the "Add Bags" button
    GtkWidget *addBagsButton = gtk_button_new_with_label("Add Bags");
    g_signal_connect(addBagsButton, "clicked", G_CALLBACK(addBagsClicked), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), addBagsButton, FALSE, FALSE, 0);

    // Create the "Create Ticket" button
    GtkWidget *createTicketButton = gtk_button_new_with_label("Create Ticket");
    g_signal_connect(createTicketButton, "clicked", G_CALLBACK(createTicketClicked), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), createTicketButton, FALSE, FALSE, 0);

    // Show all the widgets
    gtk_widget_show_all(window);

    // Establish connection to the PostgreSQL database
    conn = PQconnectdb("dbname=mydb user=myuser password=mypassword");
    if (PQstatus(conn) != CONNECTION_OK) {
        printf("Connection to database failed: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return 1;
    }

    // Start the GTK main loop
    gtk_main();

    // Clean up and close the database connection
    PQfinish(conn);

    return 0;
}
