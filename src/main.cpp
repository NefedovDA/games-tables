#include <iostream>
#include <unordered_set>

namespace {

struct Table {
  size_t index{};
  size_t seats_count{};
  std::unordered_set<size_t> persons;

  static bool CompBySeats(const Table& lhs, const Table& rhs) {
    return lhs.seats_count > rhs.seats_count;
  }

  static bool CompareByIndex(const Table& lhs, const Table& rhs) {
    return lhs.index < rhs.index;
  }
};

std::vector<Table> TablesFromSeats(const std::vector<size_t>& tables_seats) {
  auto tables = std::vector<Table>();
  tables.reserve(tables_seats.size());
  size_t index = 1;
  std::transform(tables_seats.begin(), tables_seats.end(),
                 std::back_inserter(tables), [&index](size_t seats_count) {
                   return Table{.index = index++, .seats_count = seats_count};
                 });
  return tables;
}

std::ostream& operator<<(std::ostream& output, const Table& table) {
  auto persons =
      std::vector<size_t>(table.persons.begin(), table.persons.end());
  std::sort(persons.begin(), persons.end());
  output << "table №" << table.index << ":";
  for (const auto person : persons) {
    output << " " << person + 1;
  }
  return output;
}

struct Person {
  size_t index{};
  std::unordered_set<size_t> opponents;
};

std::vector<Person> MakePersons(size_t n) {
  auto persons = std::vector<Person>(n);
  size_t index = 0;
  for (auto& person : persons) {
    person.index = index++;
  }
  return persons;
}

template <typename T>
std::vector<T> ReadVector(std::istream& input, size_t count) {
  auto result = std::vector<T>();
  result.reserve(count);
  std::copy_n(std::istream_iterator<size_t>{input}, count,
              std::back_inserter(result));
  return result;
}

void SitPerson(Table& table, std::vector<Person>& persons,
               std::unordered_set<size_t>& used) {
  const auto get_new_opponents = [&table](const Person& person) {
    auto new_opponents = std::unordered_set<size_t>();
    for (size_t opponent : table.persons) {
      if (!person.opponents.contains(opponent)) {
        new_opponents.insert(opponent);
      }
    }
    return new_opponents;
  };

  const auto first_unused_person = [&used, &persons]() -> Person& {
    return *std::find_if(
        persons.begin(), persons.end(),
        [&used](const Person& person) { return !used.contains(person.index); });
  };
  auto& first_person = first_unused_person();

  auto beast_person = std::ref(first_person);
  auto beast_new_opponents = get_new_opponents(first_person);

  for (auto& person : persons) {
    if (used.contains(person.index)) continue;
    auto new_opponents = get_new_opponents(person);
    if (new_opponents.size() > beast_new_opponents.size() ||
        (new_opponents.size() == beast_new_opponents.size() &&
         person.opponents.size() < beast_person.get().opponents.size())) {
      beast_person = person;
      beast_new_opponents = std::move(new_opponents);
    }
  }

  auto& person = beast_person.get();
  table.persons.insert(person.index);
  used.insert(person.index);
  for (auto opponent : beast_new_opponents) {
    person.opponents.insert(opponent);
    persons[opponent].opponents.insert(person.index);
  }
}

void MakeArrangements(std::vector<Person>& persons,
                      std::vector<Table>& tables) {
  auto used = std::unordered_set<size_t>();
  for (auto& table : tables) {
    for (size_t i = 0; i < table.seats_count; ++i) {
      SitPerson(table, persons, used);
    }
  }
}

}  // namespace

int main() {
  size_t n, m, k;
  std::cin >> n >> k >> m;

  auto empty_tables = TablesFromSeats(ReadVector<size_t>(std::cin, k));
  std::sort(empty_tables.begin(), empty_tables.end(), &Table::CompBySeats);

  auto persons = MakePersons(n);

  auto seating_arrangements = std::vector<std::vector<Table>>();
  for (size_t i = 0; i < m; ++i) {
    MakeArrangements(persons, seating_arrangements.emplace_back(empty_tables));
  }

  size_t day = 1;
  for (auto& tables : seating_arrangements) {
    std::cout << "Day №" << day++ << std::endl;
    std::sort(tables.begin(), tables.end(), &Table::CompareByIndex);
    for (const auto& table : tables) {
      std::cout << table << std::endl;
    }
    std::cout << std::endl;
  }

  size_t score = 0;
  std::cout << "Persons:" << std::endl;
  for (const auto& person : persons) {
    std::cout << "Person №" << person.index << ": " << person.opponents.size()
              << " |";
    auto opponents =
        std::vector<size_t>(person.opponents.begin(), person.opponents.end());
    std::sort(opponents.begin(), opponents.end());
    for (size_t opponent : opponents) {
      std::cout << " " << opponent;
    }
    std::cout << std::endl;

    score += opponents.size();
  }

  std::cout << std::endl << "Total score: " << score << std::endl;

  return 0;
}
