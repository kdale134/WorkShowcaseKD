const fs = require('fs');
const meal = JSON.parse(fs.readFileSync('./data/meal.json', 'utf8'));

const meals = (req, res) => {
    pageTitle = 'Travlr Getaways - Meals';
    res.render('meals', {title: pageTitle, meal});
};

module.exports = {
    meals
}