![run-tests](../../workflows/run-tests/badge.svg)

Γρηγορίου Κωνσταντίνα 1115201700025

Πολίτης Δημήτριος 1115201700128

Φλύρη Παναγιώτα 1115200900271


Για να το τρέξουμε χρησιμοποιούμε το runTestharness.sh του submission ή το make run που το κάνει "χειροκίνητα".
Τα tests τρέχουν με make tests. Tο executable λέγεται exec.

Partitiones Hash Join
Δέχεται τα δυο relation και κάθε φορά επιλέγει αυτό με τα λιγότερα περάσματα στο partition ώστε να δημιουργηθεί το hash table. Σε περίπτωση που έχουν περάσει και τα δύο από δυο φορές τότε επιλέγει το πρώτο.

#### Hopscotch στα relations
- 0 pass
  ένας απλός πίνακας για το hash table
- 1 pass
  πίνακας από δείκτες σε hopscoth hash table 
- 2 pass 
  δισδιαστατος πίνακας από hash tables
Hopscotch

Στο hopscotch implementation μας το H είναι μεταβλητό. Κάθε φορά που θα χρειάζεται να μεγαλώσει το hopscotch, διπλασιάζεται το Ν και το Η. Αυτό σημαίνει ότι ανάλογα με το πόσα resize θα χρειαστούν θα μεγλώσει αρκετά το H.

#### Queries

Το πρόγραμμα δέχεται τα queries από το stdin ανά batch. Τις πληροφορίες για κάθε query τις παίρνει καλόντας την συνάρτηση get_batch_info() η όποια επιστρέφει ένα πίνακα από queries. Στη συνέχεια χρησιμοποιεί τα relation ID, τα φίλτρα, τα join και τα sum όπου είναι απαραίτητο. Μόλις τελειώσει το συγκεκριμένο batch, γινεται free ο πίνακας και καλείται ξανά η get_batch_info. Η διαδικασία αυτή επαναλαμβάνεται μέχρι να εεπιστρέψει NULL.





Partition 

Στη συνάρτηση partition της εργασίας έχουμε υλοποιήσει τη διαδικασία με την οποία αναδιατάσεται ένας πίνακας (initialRelation) ή ένα κόμματι αυτού του πίνακα. Πιο συγκεκριμένα, η συνάρτηση δέχεται ένα πίνακα R και δημιουργεί ένα ιστόγραμμα όπου καταγράφεται το πλήθος των στοιχείων που υπάρχουν στον αρχικό κάδο. Στη συνέχεια, δημιουργούμε τον πίνακα Psum όπου αποθηκεύεται η πληροφορία σχετικά με το από που ξεκινάει ο κάθε κάδος. Επίσης, έχουμε χρησιμοποιήσει ακόμη έναν πίνακα currentIndex ώστε να γνωρίζουμε σε ποιά θέση του νέου reordered πινακα μπορούμε να τοποθετήσουμε νέο στοιχείο.Τέλος με αυτό τον τρόπο χτίζουμε το νέο reordered πίνακα R'.
Χρησιμοποιώντας το pSum που μας επέστρεψε το x βήμα του partition μπορούμε να τρέξουμε το x+1 βήμα δίνοντας κάθε φορά από που ξεκινάει και που τελειώνει το κάθε μοναδικό στοιχείο (startIndex, endIndex).

Αναλυτικά τα ορίσματα:
struct relation initialRelation: είναι ο αρχικός πίνακας με τα δεδομένα μας
int startIndex: δείχνει από που να ξεκινήσουμε να κάνουμε την αναδιάταξη
int endIndex: δείχνει που να σταματήσουμε να κάνουμε την αναδιάταξη
int numberOfBytes: ορίζει σε πόσα byte γίνεται το partition 
int partitionStep: δείχνει ποιο βήμα του partition να κάνουμε, χρησιμοποιείται για να ξέρουμε σε ποιά byte να κάνουμε partition π.χ. για numberOfBytes=3 το πρώτο βήμα θα πάρει τα 3 τελευταία byte, το δεύτερο βήμα τα 3 προτελευταία bytes κ.τ.λ.
struct relation* reOrdered: εδώ θα επιστρέψουμε το νέο πίνακα που θα προκύψει μετά το reorder
int* max: εδώ θα επιστρέψουμε το μεγαλύτερο πλήθος στοιχείων που βρίσκεται σε κάποια θέση του ιστογράμματος
int** pSum: εδώ θα επιστρέψουμε το αθροιστικό ιστόγραμμα που δείχνει από που ξεκινά ο κάθε κάδος
