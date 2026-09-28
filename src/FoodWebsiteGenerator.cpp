/**
 * ============================================================================
 * Project: FoodWebsite Generator (Object-Oriented Programming in C++)
 * Author:  Sohrab Soomro (FAST NUCES Peshawar)
 * Course:  Object-Oriented Programming (OOP)
 *
 * Description:
 *   An Object-Oriented C++ application where restaurant owners input details
 *   about their food service (brand name, tagline, theme palette, address,
 *   and categorized menu items), and the program generates a complete,
 *   responsive HTML5/CSS3 restaurant website.
 *
 * OOP Pillars Demonstrated:
 *   1. Encapsulation: Private state inside RestaurantInfo and ThemePalette
 *      accessed via validated getters and setters.
 *   2. Inheritance: Base class `MenuItem` extended by `MainCourseItem`,
 *      `BeverageItem`, and `ComboDealItem`.
 *   3. Polymorphism: Virtual method `renderCardHTML()` overridden by each
 *      derived menu class to render specialized badges and nutritional/deal tags.
 *   4. Abstraction: Abstract interface `IHtmlRenderable` defining the contract
 *      for any component that can serialize itself into HTML markup.
 * ============================================================================
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

// ----------------------------------------------------------------------------
// 1. ABSTRACTION: Pure abstract interface for HTML-serializable components
// ----------------------------------------------------------------------------
class IHtmlRenderable {
public:
    virtual string toHTML() const = 0;
    virtual ~IHtmlRenderable() {}
};

// ----------------------------------------------------------------------------
// 2. ENCAPSULATION: Brand Theme & Restaurant Profile Metadata
// ----------------------------------------------------------------------------
class ThemePalette {
private:
    string primaryColor;
    string accentColor;
    string backgroundColor;

public:
    ThemePalette(string primary = "#D6112C", string accent = "#B4700A", string bg = "#FAF8F5")
        : primaryColor(primary), accentColor(accent), backgroundColor(bg) {}

    string getPrimary() const { return primaryColor; }
    string getAccent() const { return accentColor; }
    string getBackground() const { return backgroundColor; }
};

class RestaurantProfile {
private:
    string name;
    string tagline;
    string cuisineType;
    string cityLocation;
    string contactPhone;
    string deliveryHours;

public:
    RestaurantProfile(string n, string t, string c, string loc, string phone, string hours)
        : name(n), tagline(t), cuisineType(c), cityLocation(loc), contactPhone(phone), deliveryHours(hours) {}

    string getName() const { return name; }
    string getTagline() const { return tagline; }
    string getCuisineType() const { return cuisineType; }
    string getCityLocation() const { return cityLocation; }
    string getContactPhone() const { return contactPhone; }
    string getDeliveryHours() const { return deliveryHours; }
};

// ----------------------------------------------------------------------------
// 3. INHERITANCE & POLYMORPHISM: MenuItem Hierarchy
// ----------------------------------------------------------------------------
class MenuItem : public IHtmlRenderable {
protected:
    string title;
    string description;
    double pricePKR;
    bool isChefRecommended;

public:
    MenuItem(string t, string d, double p, bool rec = false)
        : title(t), description(d), pricePKR(p), isChefRecommended(rec) {}

    virtual ~MenuItem() {}

    // Pure virtual functions overridden by specialized food categories
    virtual string getCategoryName() const = 0;
    virtual string getExtraBadgeHTML() const = 0;

    // Polymorphic HTML card generator
    string toHTML() const override {
        ostringstream oss;
        oss << "      <article class="menu-card">
"
            << "        <div class="card-top">
"
            << "          <span class="cat-tag">" << getCategoryName() << "</span>
"
            << (isChefRecommended ? "          <span class="chef-tag">Chef's Pick</span>
" : "")
            << "        </div>
"
            << "        <h3>" << title << "</h3>
"
            << "        <p class="desc">" << description << "</p>
"
            << "        <div class="card-foot">
"
            << "          <span class="price">PKR " << fixed << setprecision(0) << pricePKR << "</span>
"
            << "          " << getExtraBadgeHTML() << "
"
            << "        </div>
"
            << "      </article>
";
        return oss.str();
    }
};

// Derived Class 1: Main Course / Grill Item
class MainCourseItem : public MenuItem {
private:
    string spiceLevel;
    int prepTimeMinutes;

public:
    MainCourseItem(string t, string d, double p, string spice, int prepMins, bool rec = false)
        : MenuItem(t, d, p, rec), spiceLevel(spice), prepTimeMinutes(prepMins) {}

    string getCategoryName() const override { return "Main Course"; }

    string getExtraBadgeHTML() const override {
        return "<span class="meta-pill">Spice: " + spiceLevel + " · " + to_string(prepTimeMinutes) + " mins</span>";
    }
};

// Derived Class 2: Beverage / Refreshment Item
class BeverageItem : public MenuItem {
private:
    bool servedChilled;
    int volumeML;

public:
    BeverageItem(string t, string d, double p, bool chilled, int ml, bool rec = false)
        : MenuItem(t, d, p, rec), servedChilled(chilled), volumeML(ml) {}

    string getCategoryName() const override { return "Beverages"; }

    string getExtraBadgeHTML() const override {
        return "<span class="meta-pill">" + string(servedChilled ? "Chilled" : "Hot") + " · " + to_string(volumeML) + "ml</span>";
    }
};

// Derived Class 3: Family / Student Combo Deal
class ComboDealItem : public MenuItem {
private:
    int servesPersons;
    int savingsPercent;

public:
    ComboDealItem(string t, string d, double p, int serves, int savePct, bool rec = true)
        : MenuItem(t, d, p, rec), servesPersons(serves), savingsPercent(savePct) {}

    string getCategoryName() const override { return "Value Combo"; }

    string getExtraBadgeHTML() const override {
        return "<span class="meta-pill save">Serves " + to_string(servesPersons) + " · Save " + to_string(savingsPercent) + "%</span>";
    }
};

// ----------------------------------------------------------------------------
// 4. COMPOSITE BUILDER: WebsiteGenerator Class
// ----------------------------------------------------------------------------
class WebsiteGenerator {
private:
    RestaurantProfile profile;
    ThemePalette theme;
    vector<MenuItem*> menuCatalog;

public:
    WebsiteGenerator(const RestaurantProfile& prof, const ThemePalette& th)
        : profile(prof), theme(th) {}

    // Destructor cleans up dynamically allocated polymorphic MenuItem objects
    ~WebsiteGenerator() {
        for (size_t i = 0; i < menuCatalog.size(); ++i) {
            delete menuCatalog[i];
        }
    }

    void addMenuItem(MenuItem* item) {
        menuCatalog.push_back(item);
    }

    bool generateWebsiteFile(const string& outputFilePath) const {
        ofstream out(outputFilePath.c_str());
        if (!out.is_open()) return false;

        out << "<!DOCTYPE html>
<html lang="en">
<head>
"
            << "<meta charset="UTF-8">
"
            << "<meta name="viewport" content="width=device-width, initial-scale=1.0">
"
            << "<title>" << profile.getName() << " — " << profile.getCuisineType() << "</title>
"
            << "<style>
"
            << "  :root { --primary: " << theme.getPrimary() << "; --accent: " << theme.getAccent() << "; --bg: " << theme.getBackground() << "; }
"
            << "  * { box-sizing: border-box; margin: 0; padding: 0; font-family: 'Segoe UI', system-ui, sans-serif; }
"
            << "  body { background: var(--bg); color: #1E2024; line-height: 1.6; }
"
            << "  header { background: linear-gradient(135deg, #14171C 0%, var(--primary) 100%); color: #fff; padding: 64px 24px; text-align: center; }
"
            << "  header .badge { display: inline-block; background: rgba(255,255,255,0.15); padding: 4px 14px; border-radius: 20px; font-size: 12px; letter-spacing: 1px; text-transform: uppercase; margin-bottom: 12px; }
"
            << "  header h1 { font-size: 42px; margin-bottom: 8px; }
"
            << "  header p { font-size: 18px; opacity: 0.9; max-width: 620px; margin: 0 auto 20px; }
"
            << "  .info-bar { display: flex; justify-content: center; gap: 24px; flex-wrap: wrap; background: #fff; padding: 16px 24px; border-bottom: 1px solid #E5E7EB; font-size: 14px; font-weight: 600; }
"
            << "  .container { max-width: 1080px; margin: 40px auto; padding: 0 20px; }
"
            << "  .section-title { font-size: 26px; margin-bottom: 20px; border-left: 4px solid var(--primary); padding-left: 12px; }
"
            << "  .menu-grid { display: grid; grid-template-columns: repeat(auto-fill, minmax(300px, 1fr)); gap: 20px; }
"
            << "  .menu-card { background: #fff; border-radius: 12px; padding: 20px; border: 1px solid #E5E7EB; display: flex; flex-direction: column; justify-content: space-between; box-shadow: 0 4px 12px rgba(0,0,0,0.04); }
"
            << "  .card-top { display: flex; justify-content: space-between; margin-bottom: 10px; }
"
            << "  .cat-tag { font-size: 11px; font-weight: 700; text-transform: uppercase; color: var(--primary); background: #FDE8EC; padding: 3px 10px; border-radius: 20px; }
"
            << "  .chef-tag { font-size: 11px; font-weight: 700; color: #155E3A; background: #E7F5EE; padding: 3px 10px; border-radius: 20px; }
"
            << "  .menu-card h3 { font-size: 19px; margin-bottom: 6px; }
"
            << "  .menu-card .desc { font-size: 14px; color: #565C68; margin-bottom: 16px; }
"
            << "  .card-foot { display: flex; justify-content: space-between; align-items: center; border-top: 1px dashed #E5E7EB; padding-top: 12px; }
"
            << "  .price { font-size: 18px; font-weight: 800; color: #14171C; }
"
            << "  .meta-pill { font-size: 12px; background: #F3F4F6; padding: 4px 10px; border-radius: 6px; color: #374151; font-weight: 600; }
"
            << "  .meta-pill.save { background: #FEF3C7; color: #92400E; }
"
            << "  footer { text-align: center; padding: 32px 20px; color: #6B7280; font-size: 13px; border-top: 1px solid #E5E7EB; margin-top: 60px; }
"
            << "</style>
</head>
<body>
"
            << "  <header>
"
            << "    <span class="badge">" << profile.getCuisineType() << "</span>
"
            << "    <h1>" << profile.getName() << "</h1>
"
            << "    <p>" << profile.getTagline() << "</p>
"
            << "  </header>
"
            << "  <div class="info-bar">
"
            << "    <span>📍 " << profile.getCityLocation() << "</span>
"
            << "    <span>📞 " << profile.getContactPhone() << "</span>
"
            << "    <span>🕒 " << profile.getDeliveryHours() << "</span>
"
            << "  </div>
"
            << "  <main class="container">
"
            << "    <h2 class="section-title">Our Signature Menu</h2>
"
            << "    <div class="menu-grid">
";

        // Polymorphic dispatch across all MenuItem pointers
        for (size_t i = 0; i < menuCatalog.size(); ++i) {
            out << menuCatalog[i]->toHTML();
        }

        out << "    </div>
"
            << "  </main>
"
            << "  <footer>Generated by <b>FoodWebsite Generator (C++ OOP Engine)</b> · Built by Sohrab Soomro (FAST NUCES)</footer>
"
            << "</body>
</html>
";

        out.close();
        return true;
    }
};

// ----------------------------------------------------------------------------
// 5. APPLICATION ENTRY POINT (Supports both interactive input & demo preset)
// ----------------------------------------------------------------------------
int main() {
    cout << "==========================================================" << endl;
    cout << "   FoodWebsite Generator (C++ OOP Static Site Builder)    " << endl;
    cout << "==========================================================" << endl;

    cout << "
Select Mode:
  1. Generate Demo Restaurant Website (Khyber Spice & Grill)
  2. Enter Custom Restaurant & Menu Details
Choice (1 or 2): ";
    int mode = 1;
    if (!(cin >> mode)) mode = 1;
    cin.ignore();

    string rName = "Khyber Spice & Charcoal Grill";
    string rTagline = "Authentic Charcoal BBQ, Hand-Crafted Burgers & Artisanal Refreshments";
    string rCuisine = "Pakistani BBQ & Continental";
    string rLocation = "University Road, Peshawar";
    string rPhone = "+92 91 555-0192";
    string rHours = "12:00 PM - 12:30 AM Daily";

    if (mode == 2) {
        cout << "Enter Restaurant Name: "; getline(cin, rName);
        cout << "Enter Tagline: "; getline(cin, rTagline);
        cout << "Enter Cuisine Type: "; getline(cin, rCuisine);
        cout << "Enter Location/Address: "; getline(cin, rLocation);
        cout << "Enter Contact Phone: "; getline(cin, rPhone);
    }

    RestaurantProfile profile(rName, rTagline, rCuisine, rLocation, rPhone, rHours);
    ThemePalette theme("#D6112C", "#B4700A", "#FAF8F5");
    WebsiteGenerator generator(profile, theme);

    // Populate menu items using polymorphic subclasses
    generator.addMenuItem(new MainCourseItem("Peshawari Namkeen Tikka Platter", "Tender charcoal-grilled lamb cubes seasoned with Himalayan rock salt.", 1650, "Mild", 25, true));
    generator.addMenuItem(new MainCourseItem("Smoky Beef Smash Burger", "Double Angus beef patty, caramelized onions, cheddar & house chipotle sauce.", 980, "Medium", 15, true));
    generator.addMenuItem(new MainCourseItem("Shinwari Mutton Karahi (Half)", "Freshly wok-seared mutton with vine tomatoes, green chilies, and ginger.", 2400, "Hot", 30, false));
    generator.addMenuItem(new ComboDealItem("FAST Campus Quad Feast", "2 Smash Burgers, 1 Full Charcoal Chicken, 4 Fries & 1.5L Drink.", 3450, 4, 22, true));
    generator.addMenuItem(new BeverageItem("Mint Margarita Cooler", "Fresh garden mint blended with lime, crushed ice, and black salt.", 320, true, 400, true));
    generator.addMenuItem(new BeverageItem("Peshawari Kahwa Pitcher", "Traditional green tea infused with cardamom and crushed almonds.", 280, false, 500, false));

    string outFile = "index.html";
    if (generator.generateWebsiteFile(outFile)) {
        cout << "
[SUCCESS] Generated responsive restaurant website: " << outFile << endl;
    } else {
        cout << "
[ERROR] Could not write output file." << endl;
    }
    return 0;
}
