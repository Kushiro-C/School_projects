package com.example.mobile_project

import android.app.Activity
import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.view.Menu
import android.view.MenuInflater
import android.view.MenuItem
import android.widget.Toast
import androidx.activity.result.ActivityResultLauncher
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.widget.SearchView
import androidx.lifecycle.ViewModelProvider
import androidx.recyclerview.widget.LinearLayoutManager
import com.example.mobile_project.databinding.ActivityWateringBinding
import java.text.SimpleDateFormat
import java.util.*


/**
 * WateringActivity displays the plants that need watering
 */
class WateringActivity : AppCompatActivity(), AdapterCallback {
    private val binding : ActivityWateringBinding
    by lazy { ActivityWateringBinding.inflate(layoutInflater) }

    private var searchItem : MenuItem? = null
    private lateinit var model : ViewModel
    private lateinit var myRecViewAdapter : MyPlantsRecyclerViewAdapter
    private var prefix = ""
    private var listPlants : List<Plant> = listOf()
    private var listNextWatering : List<String> = listOf()

    private var launcher : ActivityResultLauncher<Intent> =
        registerForActivityResult(
            ActivityResultContracts.StartActivityForResult())
        { // listener
            if (it.resultCode == Activity.RESULT_OK) {
                val value = it.data?.extras?.get("actionOK").toString()
                if (value == "update")
                    Toast.makeText(this, R.string.plantUpdated, Toast.LENGTH_SHORT).show()
                else if (value == "delete")
                    Toast.makeText(this, R.string.plantDeleted, Toast.LENGTH_SHORT).show()
                model.loadAll()
            }
        }

    /**
     * Method called from RecyclerView adapter
     * to update a plant (for watering)
     */
    override fun updateCallback(plant: Plant, pos : Int) {
        Log.d("MethodCallback", "In WateringActivity")
        model.updateData(plant)
        model.loadAll()
        updateWateringList()
        myRecViewAdapter.notifyItemRemoved(pos)
        Toast.makeText(this, R.string.plantsWatered, Toast.LENGTH_SHORT).show()
    }

    /**
     * Put in listPlants only the Plants
     * which need watering
     * and also Updates listNextWatering
     */
    private fun updateWateringList(){
        val arrListPlant = ArrayList<Plant>()
        val arrListWatering = ArrayList<String>()

        // Init Calendars and Frequencies
        val todayCalendar = Calendar.getInstance()
        val nextWatering = Calendar.getInstance()
        val lastWatering = Calendar.getInstance()
        val seasonFrom = Calendar.getInstance()
        val seasonTo = Calendar.getInstance()
        val nutrientFrom = Calendar.getInstance()
        val nutrientTo = Calendar.getInstance()
        val simpleDateFormat = SimpleDateFormat("yyyy-MM-dd", Locale.FRENCH)

        listPlants.forEach {

            // Set Frequencies
            val freq = it.frequence
            val seasonFreq = it.frequence2 ?: 0
            val nutrientFreq = it.frequence3 ?: 0

            // Set Calendars
            lastWatering.time = simpleDateFormat.parse(it.last_watering)
            if (seasonFreq > 0) { // Not null if seasonFreq is set
                seasonFrom.time = simpleDateFormat.parse(it.season_from)
                seasonTo.time = simpleDateFormat.parse(it.season_to)
            }
            if (nutrientFreq > 0) { // Not null if nutrientFreq is set
                nutrientFrom.time = simpleDateFormat.parse(it.nutrient_from)
                nutrientTo.time = simpleDateFormat.parse(it.nutrient_to)
            }

            // Set nextWatering date
            nextWatering.time = lastWatering.time
            if (todayCalendar in nutrientFrom..nutrientTo
                && nutrientFreq > 0) // Priority on nutrient
                nextWatering.add(Calendar.DATE, nutrientFreq)

            else if (todayCalendar in seasonFrom..seasonTo
                && seasonFreq > 0) // Priority season
                nextWatering.add(Calendar.DATE, seasonFreq)

            else // Normal watering frequency
                nextWatering.add(Calendar.DATE, freq)

            // Check if we are past the nextWatering date
            if (todayCalendar.after(nextWatering)) {
                arrListPlant.add(it)
                arrListWatering.add(simpleDateFormat.format(nextWatering.time))
            }
        }
        listPlants = arrListPlant.toList()
        listNextWatering = arrListWatering.toList()
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(binding.root)
        setSupportActionBar(binding.myToolbar)

        /* Back button in menu toolbar */
        supportActionBar?.setDisplayHomeAsUpEnabled(true)

        model = ViewModelProvider(this).get(ViewModel::class.java)

        val recyclerView = binding.recyclerView
        recyclerView.layoutManager = LinearLayoutManager(this)
        (recyclerView.layoutManager as LinearLayoutManager?)!!.stackFromEnd = true
        updateWateringList()

        myRecViewAdapter = MyPlantsRecyclerViewAdapter(
            listPlants, listNextWatering, prefix, launcher, this)
        recyclerView.adapter = myRecViewAdapter

        // Updates the recyclerView whenever the list changes
        model.certainsPlants.observe(this) {
            listPlants = it
            updateWateringList()
            myRecViewAdapter = MyPlantsRecyclerViewAdapter(
                listPlants, listNextWatering, prefix, launcher, this)
            recyclerView.adapter = myRecViewAdapter
        }
    }

    override fun onSaveInstanceState(outState: Bundle) {
        super.onSaveInstanceState(outState)
        outState.putString("search", prefix)
    }

    override fun onRestoreInstanceState(savedInstanceState: Bundle) {
        super.onRestoreInstanceState(savedInstanceState)
        prefix = savedInstanceState.getString("search") ?: ""
    }

    /**
     * Menu Toolbar
     */
    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        val inflater: MenuInflater = menuInflater
        inflater.inflate(R.menu.activity_bis_menu, menu)

        /* Search action */
        searchItem  = menu.findItem(R.id.action_search)
        val searchView = searchItem?.actionView as SearchView

        // Restore searched name
        if (searchItem != null && prefix != "") {
            searchView.setQuery(prefix, true)
            searchView.clearFocus()

            // Restore recyclerView
            model.partialNomPlant(prefix)
        }

        searchView.setOnQueryTextListener(object :
            SearchView.OnQueryTextListener {
            override fun onQueryTextSubmit(s: String): Boolean {
                prefix = s
                model.partialNomPlant(s)

                return true
            }
            override fun onQueryTextChange(s: String): Boolean {
                prefix = s
                model.partialNomPlant(s)

                return true
            }
        })
        /* must always return true */
        return true
    }
}