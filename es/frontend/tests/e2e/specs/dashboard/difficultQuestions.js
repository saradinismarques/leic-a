describe('Difficult Questions', () => {
    const date = new Date();
    beforeEach(() => {
      cy.deleteQuestionsAndAnswers();
      //create quiz
      cy.demoTeacherLogin();
      cy.createQuestion(
        'Question Difficult Question 1 ' + date.toString(),
        'Question',
        'Option',
        'Option',
        'ChooseThisWrong',
        'Correct'
      );
      cy.createQuestion(
        'Question Difficult Question 2 ' + date.toString(),
        'Question',
        'Option',
        'Option',
        'ChooseThisWrong',
        'Correct'
      );
      cy.createQuizzWith2Questions(
        'Difficult Question Title ' + date.toString(),
        'Question Difficult Question 1 ' + date.toString(),
        'Question Difficult Question 2 ' + date.toString()
      );
      cy.contains('Logout').click();
    });
  
    it('student accesses difficult questions', () => {
      cy.demoStudentLogin();
      cy.solveQuizDifficultQuestions('Difficult Question Title ' + date.toString(), 2);
      cy.updateDifficultQuestions();
      cy.contains('Logout').click();
      Cypress.on('uncaught:exception', (err, runnable) => {
        // returning false here prevents Cypress from
        // failing the test
        return false;
      });
    });
    afterEach(() => {
        cy.deleteDifficultQuestions();
        cy.deleteQuestionsAndAnswers();
      });
  });