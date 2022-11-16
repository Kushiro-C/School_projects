package com.example.mobile_project

import android.annotation.SuppressLint
import android.app.Activity
import android.content.Intent
import android.os.Bundle
import android.text.Editable
import android.text.TextWatcher
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.EditText
import android.widget.Toast
import androidx.activity.result.ActivityResultLauncher
import androidx.activity.result.contract.ActivityResultContracts
import androidx.fragment.app.Fragment
import androidx.fragment.app.activityViewModels
import androidx.recyclerview.widget.LinearLayoutManager
import com.example.mobile_project.databinding.FragmentDisplayPlantsBinding
import java.text.SimpleDateFormat
import java.util.*


/**
 * Fragment class to Display all the Plants
 */
class DisplayPlantsFragment : Fragment(R.layout.fragment_display_plants), AdapterCallback {

    private val model : ViewModel by activityViewModels()
    private lateinit var myRecViewAdapter : MyPlantsRecyclerViewAdapter
    private var listPlants : List<Plant> = listOf()
    private var listNextWatering : List<String> = listOf()
    private var prefix : String = "" // Text in the search bar
    private lateinit var binding : FragmentDisplayPlantsBinding

    private var launcher : ActivityResultLauncher<Intent> =
        registerForActivityResult(
            ActivityResultContracts.StartActivityForResult())
        { // listener
            if (it.resultCode == Activity.RESULT_OK) {
                val value = it.data?.extras?.get("actionOK").toString()
                if (value == "update")
                    Toast.makeText(activity, R.string.plantUpdated, Toast.LENGTH_SHORT).show()
                else if (value == "delete")
                    Toast.makeText(activity, R.string.plantDeleted, Toast.LENGTH_SHORT).show()
                model.loadAll()
            }
        }

    /**
     * Method called from RecyclerView adapter
     * to update a plant (for watering)
     */
    override fun updateCallback(plant: Plant, pos : Int) {
        Log.d("MethodCallback", "In Fragment Display")
        model.updateData(plant)
        model.loadAll()
        myRecViewAdapter.notifyItemChanged(pos)
        Toast.makeText(activity, R.string.plantsWatered, Toast.LENGTH_SHORT).show()
    }

    /**
     * Updates listNextWatering
     */
    private fun updateNextWateringList(){
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

            arrListWatering.add(simpleDateFormat.format(nextWatering.time))
        }
        listNextWatering = arrListWatering.toList()
    }

    @SuppressLint("LongLogTag")
    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        val view = inflater.inflate(R.layout.fragment_display_plants, container, false)
        binding = FragmentDisplayPlantsBinding.bind( view )

        val recyclerView = binding.recyclerView
        recyclerView.layoutManager = LinearLayoutManager(context)
        (recyclerView.layoutManager as LinearLayoutManager?)!!.stackFromEnd = true
        model.loadAll()
        myRecViewAdapter = MyPlantsRecyclerViewAdapter(
            listPlants, listNextWatering, prefix, launcher, this)
        recyclerView.adapter = myRecViewAdapter

        model.certainsPlants.observe(viewLifecycleOwner) {
            listPlants = it
            updateNextWateringList()
            myRecViewAdapter = MyPlantsRecyclerViewAdapter(
                it, listNextWatering, prefix, launcher, this)
            recyclerView.adapter = myRecViewAdapter
            // Add Toast to check, or Log or print
            Log.d("observe", "in observer display plants fragment")
        }

        // EditText search bar settings
        val recherche : EditText = binding.search // Search bar
        recherche.addTextChangedListener(object : TextWatcher {
            override fun beforeTextChanged(p0: CharSequence?, p1: Int, p2: Int, p3: Int) {
            }

            override fun onTextChanged(p0: CharSequence?, p1: Int, p2: Int, p3: Int) {
            }

            // Whenever changing EditText, we do a query with the text for 'plant'
            override fun afterTextChanged(editable: Editable?) {
                model.partialNomPlant(editable!!.toString())
                prefix = editable.toString()
            }
        })
        return view
    }


    companion object {
        @JvmStatic
        fun newInstance() =
            DisplayPlantsFragment().apply {
                arguments = Bundle().apply {
                }
            }
    }
}

